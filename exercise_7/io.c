#include "io.h"

static comedi_t *it_g = NULL;

int __attribute__ ((constructor)) io_init(){
    int i;
    int res;

    it_g = comedi_open("/dev/comedi0");

    if (it_g == NULL) return -1;

    // init 3 inputs and 3 outputs
    for(i=1; i<4; i++)
    {
        res = comedi_dio_config(it_g, IO_DEV, DI(i), COMEDI_INPUT);
        if (res<0) return res;
        res = comedi_dio_config(it_g, IO_DEV, DO(i), COMEDI_OUTPUT);
        if (res<0) return res;
        res = comedi_dio_write(it_g, IO_DEV, DO(i), 0);
        if (res<0) return res;
    }

    // set outputs to high
    for(i=1; i<4; i++)
    {
        comedi_dio_write(it_g, IO_DEV, DO(i), 1);
    }

    printf("init complete\n");

    return 1;
}

void io_write(int channel, int value)
{
    if(channel >= 1 && channel <= 3)
    {
        if(value == 0) comedi_dio_write(it_g, IO_DEV, DO(channel), 0);
        else comedi_dio_write(it_g, IO_DEV, DO(channel), 1);
    }
    else
    {
        printf("Incorrect io channel\n");
    }
}

int io_read(int channel)
{
    unsigned int data=0;

    if(channel >= 1 && channel <= 3)
    {
        comedi_dio_read(it_g, IO_DEV, DI(channel), &data);
        return (int)data;
    }
    else
    {
        printf("Incorrect io channel\n");
        return -1;
    }


}
