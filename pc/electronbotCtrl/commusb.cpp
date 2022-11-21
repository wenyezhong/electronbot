#include "commusb.h"
#include"QDebug"
commUSB::commUSB()
{
    int r;
    size_t cnt;
    r = libusb_init(NULL);
    if(r < 0)
        qDebug() << r << endl;
    cnt = libusb_get_device_list(NULL,&devs);
    if(cnt < 0)
        libusb_exit(NULL);
        qDebug() << cnt << endl;

}
commUSB::~commUSB()
{
    libusb_free_device_list(devs,1);
    libusb_exit(NULL);
}
void commUSB::print_dev(void)
{
    libusb_device *dev;
    int i = 0;
    int j = 0;
    uint8_t path[8];
    while((dev = devs[i++]) != NULL)
    {
        struct libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev,&desc);
        if(r < 0)
        {
            qDebug() << "failed to get device descriptor.";
            break;
        }
        qDebug("%04x:%04x (bus %d, device %d)",desc.idVendor,desc.idProduct,
                libusb_get_bus_number(dev),libusb_get_device_address(dev));
        r = libusb_get_port_numbers(dev,path,sizeof(path));
        if(r > 0)
        {
            qDebug("path : %d",path[0]);
            for(j = 1;j < r;j++)
                qDebug(".%d",path[j]);
        }
    }
}

void commUSB::test_my_usb_devices(int vid,int pid)
{
    libusb_device_handle *handle;
    libusb_device *dev;
    int bus;

    struct libusb_config_descriptor *conf_desc;
    const struct libusb_endpoint_descriptor *endpoint;
    uint8_t endpoint_in = 0, endpoint_out = 0;
    int endpoint_num, nb_ifaces, altesetting = -1;
    struct libusb_ss_endpoint_companion_descriptor *ep_comp = NULL;

    uint8_t buf[64]={0};
    int i,j;
    int res;
    int size=0;

    /*打开设备*/
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (handle == NULL)
        qDebug("open devices failed");


    dev = libusb_get_device(handle);
    bus = libusb_get_bus_number(dev);
    libusb_get_config_descriptor(dev, 0, &conf_desc);
    nb_ifaces = conf_desc->bNumInterfaces;
    qDebug()<<nb_ifaces;
    if(nb_ifaces>0)
    {
    for(i = 0; i< nb_ifaces;i++)

      {
          altesetting=conf_desc->interface[i].num_altsetting;

          if(altesetting>0)
          {
              endpoint_num=conf_desc->interface[i].altsetting[0].bNumEndpoints;
              qDebug("endpoint_num = %d",endpoint_num);
              if(endpoint_num>0)
              {
                  for(j=0;j<endpoint_num;j++)
                  {
                      qDebug("tt");
                      endpoint= &conf_desc->interface[i].altsetting[0].endpoint[j];
                      if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) & (LIBUSB_TRANSFER_TYPE_BULK | LIBUSB_TRANSFER_TYPE_INTERRUPT)) {
                          qDebug("bEndpointAddress = %x",endpoint->bEndpointAddress);
//                      if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) & (LIBUSB_TRANSFER_TYPE_BULK)) {
                          if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
                              //if (!endpoint_in)
                              {
                                  endpoint_in = endpoint->bEndpointAddress;
                                  qDebug("endpoint_in = %x",endpoint_in);
                              }
                          }
                          else {
                              //if (!endpoint_out)
                              {
                                  endpoint_out = endpoint->bEndpointAddress;
                                  qDebug("endpoint_out = %x",endpoint_out);
                              }
                          }
                      }
                  }
              }
              else
                  qDebug("endpoint num error");
          }
          else
          {
              qDebug("altsetting error");
          }

      }
    }
      else
          qDebug("interface found error");

      /*释放配置描述符*/
      libusb_free_config_descriptor(conf_desc);
      /*卸载驱动内核*/
      libusb_set_auto_detach_kernel_driver(handle, 1);
      /*为指定的设备申请接口*/
      res=libusb_claim_interface(handle, 0);
      res<0?qDebug("claim interface error"):qDebug("claim interface success");
      int cnt=0;
      while (1)
      {
          if(cnt++>1)
              break;
          memset(&buf[0], 0, 63);

          qDebug("Testing interrupt read using endpoint %02X...\n", endpoint_in);
//          res = libusb_bulk_transfer(handle,0x83, buf, 64, &size, 1000);
          res = libusb_bulk_transfer(handle, endpoint_in, buf, 64, &size, 1000);
          if (res < 0)
            qDebug("bulk transfer error\n");
          if (res >= 0) {
              qDebug("read ok:");
              for (uint8_t i = 0; i < 64; i++)
              {
                  qDebug("%02x ", buf[i]);
              }

          }
          else
          {
              qDebug("   %s\n", libusb_strerror((enum libusb_error)res));
          }

          buf[0] = 0x12;
          buf[1] = 0x13;
          buf[2] = 0x14;
          buf[3] = 0x15;
          qDebug("\nTesting interrupt write using endpoint %02X...\n", endpoint_out);
//          res = libusb_interrupt_transfer(handle, endpoint_out, buf, 63, &size, 1000);
          res = libusb_bulk_transfer(handle, endpoint_out, buf, 63, &size, 1000);
          //r = libusb_interrupt_transfer(handle, endpoint_out, report_buffer, 64, &size, 1000);
          //r = libusb_interrupt_transfer(handle, endpoint_out, report_buffer, 65, &size, 1000);

          if (res >= 0) {
              qDebug("write ok\n");
          }
          else {
              qDebug("   %s\n", libusb_strerror((enum libusb_error)res));
          }
          Sleep(1000);
      }

}
