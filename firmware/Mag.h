
#define MAG_ADDR 0x1E //0011110b, I2C 7bit address of HMC5883

#ifndef MAG_SCL
#define MAG_SCL   PB2
#endif
#ifndef MAG_SDA
#define MAG_SDA   PB0
#endif

#define MAG_WRITE_HIGH(PORT) PORTB |= (1 << PORT)
#define MAG_WRITE_LOW(PORT) PORTB &= ~(1 << PORT)

void mag_start(void) {
  MAG_WRITE_HIGH(MAG_SCL);  // Set to HIGH
  MAG_WRITE_HIGH(MAG_SDA);  // Set to HIGH
  MAG_WRITE_LOW(MAG_SDA);   // Set to LOW
  MAG_WRITE_LOW(MAG_SCL);   // Set to LOW
}

void mag_end(void) {
  MAG_WRITE_LOW(MAG_SCL);   // Set to LOW
  MAG_WRITE_LOW(MAG_SDA);   // Set to LOW
  MAG_WRITE_HIGH(MAG_SCL);  // Set to HIGH
  MAG_WRITE_HIGH(MAG_SDA);  // Set to HIGH
}

void mag_send(uint8_t byte) {
  uint8_t i;
  for (i = 0; i < 8; ++i) {
    if ((byte << i) & 0x80)
      MAG_WRITE_HIGH(MAG_SDA);
    else
      MAG_WRITE_LOW(MAG_SDA);
    
    MAG_WRITE_HIGH(MAG_SCL);
    MAG_WRITE_LOW(MAG_SCL);
  }
  MAG_WRITE_HIGH(MAG_SDA);
  MAG_WRITE_HIGH(MAG_SCL);
  MAG_WRITE_LOW(MAG_SCL);
}

void mag_init() {
  DDRB |= (1 << MAG_SDA); // Set port as output
  DDRB |= (1 << MAG_SCL); // Set port as output
  mag_start();
  mag_send(MAG_ADDR);
  mag_send(0x02); //select mode register
  mag_send(0x00); //continuous measurement mode
  mag_end();
}


void mag_read(int &x, int &y, int &z) {
  DDRB |= (1 << MAG_SDA); // Set port as output
  DDRB |= (1 << MAG_SCL); // Set port as output
  mag_start();
  mag_send(MAG_ADDR);
  // Tell the HMC5883L where to begin reading data
  // with register 3 (the X MSB register)
  mag_send(0x03);
  mag_end();

  /*
   * 
  //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.receive()<<8; //X msb
    x |= Wire.receive(); //X lsb
    z = Wire.receive()<<8; //Z msb
    z |= Wire.receive(); //Z lsb
    y = Wire.receive()<<8; //Y msb
    y |= Wire.receive(); //Y lsb
  }
  */
}


