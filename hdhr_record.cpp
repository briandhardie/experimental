#include <hdhomerun.h>

hdhomerun_device_t     *_hdhomerun_device;

int main(int argc, char** argv) {
  _hdhomerun_device = get_device();
  hdhomerun_device_stream_start(_hdhomerun_device);

  return 0;
}

struct hdhomerun_device_t *get_device() {
  struct hdhomerun_discover_device_t device;
  struct hdhomerun_device_t *hd;

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



}

char *get_discovered_device_ip_as_string(struct hdhomerun_discover_device_t *device) {
  struct in_addr ip_addr;
  char *addr_str = malloc(INET_ADDRSTRLEN * sizeof(char));
  ip_addr.s_addr = ntohl(device->ip_addr);
  return inet_ntop(AF_INET, ip_addr, addr_str, INET_ADDRSTRLEN);
}

int discover_devices(struct hdhomerun_discover_device_t ** discovered_devices) {
  uint32_t target_ip = 0;
  struct hdhomerun_discover_device_t hdhr_discovered_devices[64];
  int count = hdhomerun_discover_find_devices_custom(target_ip, HDHOMERUN_DEVICE_TYPE_TUNER, HDHOMERUN_DEVICE_ID_WILDCARD, hdhr_discovered_devices, 64);

  *discovered_devices = &hdhr_discovered_devices;

  return count;
}




