#include <arpa/inet.h>
#include <netinet/in.h>
#include <hdhomerun.h>


int main(void) {
  int i = 0;
  uint32_t target_ip = 0;
  struct hdhomerun_discover_device_t discovered_devices[64];
  struct hdhomerun_discover_device_t device;
  struct hdhomerun_device_t *hd;
  int count = hdhomerun_discover_find_devices_custom(target_ip, HDHOMERUN_DEVICE_TYPE_TUNER, HDHOMERUN_DEVICE_ID_WILDCARD, discovered_devices, 64);
  printf("Devices found: %d\n", count);
  for(i=0; i<count; i++) {
    device = discovered_devices[i];
    struct in_addr ip_addr;
    ip_addr.s_addr = ntohl(device.ip_addr);
    printf("  %d @ %s\n", device.device_id, inet_ntoa(ip_addr));
  }
  if(count > 0) {
    hd = hdhomerun_device_create(discovered_devices[0].device_id, 0, 0, NULL);
    if(!hd) {
      printf("bad device, bad!");
      exit(1);
    }
    printf("Choosing first device because...why not?\n");

    const char *model = hdhomerun_device_get_model_str(hd);
    if (!model) {
      fprintf(stderr, "unable to connect to device\n");
      return -1;
    }
    printf("Connected to %s\n", model);

    printf("Setting tuner 0 channel to 8vsb:17\n");

    char *ret_error;
    if (hdhomerun_device_set_var(hd, "/tuner0/channel", "8vsb:17", NULL, &ret_error) < 0) {
      printf("Error setting channel: %s\n", ret_error);
      hdhomerun_device_destroy(hd);
      return 1;
    }

    printf("Tuner 0 channel set to 8vsb:17. Waiting 2 seconds for lock...\n");
    fflush(stdout);
    sleep(2);
    printf("Checking status of tuner 0...\n");
    
    char *ret_value;
    if(hdhomerun_device_get_var(hd, "/tuner0/status", &ret_value, &ret_error) < 0) {
      printf("Error getting status: %s\n", ret_error);
      hdhomerun_device_destroy(hd);
      return 1;
    }

    printf("Tuner status: %s\n", ret_value);

    hdhomerun_device_destroy(hd);

  }
    return 0;
}

