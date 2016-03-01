#ifndef _I2C_H_
#define _I2C_H_

#define I2C_TIMEOUT (2000)

typedef enum t_ACK_ID
{
	ACK 		= 0,
	NACK,
}ACK_ID;

typedef enum t_ADDR_TYPE
{
	ADDR_7BIT 		= 0,
	ADDR_10BIT,
} ADDR_TYPE;


/**
 * @brief Configures the specified pin to behave either as an input or an output. 
 * @param devaddr	[IN] The slave device address. It is 7 or 10 bits without r/w bit.
 * @param type		[IN] The type of slave device address.
 * @param writebuf	[IN] It is the pointer of buffer for i2c write process. The data will transfer from writebuf[0].
 * @param writelen	[IN] The data size will write. The size of writebuf must bigger than or equal to writelen.
 * @param readbuf	[IN] It is the pointer of buffer for i2c read process. The receive data will put from readbuf[0].
 * @param readlen	[INOUT] IN : The data length will read. The size of readbuf must bigger than or equal to readbuf.
 							OUT : Return the receive data size from i2c bus.
 * @return none.
 */
void i2c_rw(U16 devaddr, ADDR_TYPE type, U8 *writebuf, U8 writelen, U8 *readbuf, U8 *readlen);

#endif /* _I2C_H_ */
