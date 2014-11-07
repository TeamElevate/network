#ifndef __ELEVATE_BTLE__
#define __ELEVATE_BTLE__
typedef struct _btle_t btle_t;

btle_t* btle_new();
void    btle_destroy(btle_t** self_p);

int     btle_sock(btle_t* self);
int     btle_dev_is_up(btle_t* self);

int     btle_set_filter(btle_t* self);

int     btle_set_adv_data(btle_t* self, void* buf, size_t len);
int     btle_start_adv(btle_t* self);
int     btle_stop_adv(btle_t* self);

int     btle_start_scan(btle_t* self);
int     btle_stop_scan(btle_t* self);
#endif
