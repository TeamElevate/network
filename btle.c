#include <assert.h>              /* assert */
#include <inttypes.h>            /* uint8_t, uint16_t */
#include <bluetooth/bluetooth.h> 
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <stdlib.h>              /* malloc, free */

#include "btle.h"

struct _btle_t {
  int hci_dev_id;
  int hci_sock;
};

static int btle_set_adv_params(btle_t* self) {
  le_set_advertising_parameters_cp params;
  /* Time = 0.625 * N */
  const uint16_t FOUR_SECS = 0x1900;
  const uint16_t FIVE_SECS = 0x1F40;
  /* Not connectable, indirect advertisment */
  const uint8_t  ADV_NONCONN_IND = 0x03;

  memset(&params, 0, sizeof(params));

  params.min_interval = htobs(FOUR_SECS);
  params.max_interval = htobs(FIVE_SECS);
  params.advtype = ADV_NONCONN_IND;

  return hci_send_cmd(self->hci_sock, OGF_LE_CTL, OCF_LE_SET_ADVERTISING_PARAMETERS, sizeof(params), &params);
}

btle_t* btle_new() {
  btle_t* self = malloc(sizeof(btle_t));
  self->hci_dev_id = hci_get_route(NULL);
  if (self->hci_dev_id < 0) {
    free(self);
    return NULL;
  }

  self->hci_sock = hci_open_dev(self->hci_dev_id);
  if (self->hci_sock < 0) {
    free(self);
    return NULL;
  }
  return self;
}

void btle_destroy(btle_t** self_p) {
  assert(self_p);
  if (*self_p) {
    btle_t* self = *self_p;
    close(self->hci_sock);
    btle_stop_scan(self);
    btle_stop_adv(self);
    free(self);
    *self_p = NULL;
  }
}

int btle_sock(btle_t* self) {
  return self->hci_sock;
}

int btle_dev_is_up(btle_t* self) {
  struct hci_dev_info dev_info;
  memset(&dev_info, 0x00, sizeof(dev_info));
  dev_info.dev_id = self->hci_dev_id;
  hci_devinfo(self->hci_dev_id, &dev_info);
  return hci_test_bit(HCI_UP, &dev_info.flags);
}

int btle_set_filter(btle_t* self) {
  struct hci_filter newHciFilter;

  // setup new HCI filter
  hci_filter_clear(&newHciFilter);
  hci_filter_set_ptype(HCI_EVENT_PKT, &newHciFilter);
  hci_filter_set_event(EVT_LE_META_EVENT, &newHciFilter);
  return setsockopt(self->hci_sock, SOL_HCI, HCI_FILTER, &newHciFilter, sizeof(newHciFilter));
}

int btle_set_adv_data(btle_t* self, void* buf, size_t len) {
  le_set_advertising_data_cp data;

  if (len > sizeof(data.data)) return -1;

  data.length = len;
  memset(data.data, 0, sizeof(data.data));
  memcpy(data.data, buf, len);
  return hci_send_cmd(self->hci_sock, OGF_LE_CTL, OCF_LE_SET_ADVERTISING_DATA, sizeof(data), &data);
}

int btle_start_adv(btle_t* self) {
  int ret;
  ret = btle_set_adv_params(self);
  if (ret < 0) return -1;

  return hci_le_set_advertise_enable(self->hci_sock, 1, 1000);
}

int btle_stop_adv(btle_t* self) {
  return hci_le_set_advertise_enable(self->hci_sock, 0, 1000);
}

int btle_start_scan(btle_t* self) {
  // turn on le scan (don't filter dups)
  return hci_le_set_scan_enable(self->hci_sock, 0x01, 0, 1000);
}

int btle_stop_scan(btle_t* self) {
  return hci_le_set_scan_enable(self->hci_sock, 0x00, 0, 1000);
}
