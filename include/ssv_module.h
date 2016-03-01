#ifndef _SSV_MODULE_H
#define _SSV_MODULE_H


#define MODULE_S092  (1)     //FTDI + EVB
#define MODULE_6060Z (2)
#define MODULE_6060P (3)
//skip 4
#define MODULE_HIFLYING (5)

#define MAX_MODULE_ID (5)


//#define MODULE_ID  (MODULE_S092)

#ifndef MODULE_ID
    #error "Please define MODULE_ID!!!"
#endif

#endif //_SSV_MODULE_H
