#ifndef __ELEVATE_BTLE__
#define __ELEVATE_BTLE__
typedef struct _btle_t btle_t;

/**
 * Create a new BTLE object
 */
btle_t* btle_new();

/**
 * Destroy a BTLE object
 */
void    btle_destroy(btle_t** self_p);

/**
 * Get the socket for the BT adapter
 */
int     btle_sock(btle_t* self);

/**
 * Check that the BT adapter is up
 */
int     btle_dev_is_up(btle_t* self);

/**
 * Set the filter for the events we want from the BT adapter
 */
int     btle_set_filter(btle_t* self);

/**
 * Set the data for advertising
 */
int     btle_set_adv_data(btle_t* self, void* buf, size_t len);

/**
 * Start advertising
 */
int     btle_start_adv(btle_t* self);

/**
 * Stop advertising
 */
int     btle_stop_adv(btle_t* self);

/**
 * Start scanning for other BTLE advertisements
 */
int     btle_start_scan(btle_t* self);

/**
 * Stop scanning for other BTLE advertisements
 */
int     btle_stop_scan(btle_t* self);
#endif
