/*****************************************************************************
*
*  I2C_ServoExp.ino - Simple I2C to servo ppm signal expander.
*
*  Copyright (c) 2014 Leon Chao @ FunMaker Ltd.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*  
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.
*  
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
*
*****************************************************************************/
#include <TinyWireS.h>
#include <SoftwareServo.h> 

SoftwareServo Servo_Out[8];

unsigned char led = 10;

#define I2C_SLAVE_ADDRESS       0x20

/* Packet format :

CMD + Servo Pin + Value_1 + Value_2

total 4 bytes

*/

#define CMD_WRITE_SERVO_ANGLE   0x01

#define CMD_SET_SERVO_MIN_PULSE 0x11
#define CMD_SET_SERVO_MAX_PULSE 0x12


void setup() 
{   
    pinMode(led, OUTPUT); 

    Servo_Out[0].attach(0);
    Servo_Out[1].attach(1);
    Servo_Out[2].attach(2);
    Servo_Out[3].attach(3);
    Servo_Out[4].attach(5);
    Servo_Out[5].attach(7);
    Servo_Out[6].attach(8);
    Servo_Out[7].attach(9);

    Servo_Out[0].write(90);
    Servo_Out[1].write(90);
    Servo_Out[2].write(90);
    Servo_Out[3].write(90);
    Servo_Out[4].write(90);
    Servo_Out[5].write(90);
    Servo_Out[6].write(90);
    Servo_Out[7].write(90);

    TinyWireS.begin(I2C_SLAVE_ADDRESS);
} 

unsigned char incomming = 0;
unsigned char i2c_data[4];
union {
    unsigned int u16;
    unsigned char u8[2];
}data_u8_u16;

void i2c_cmd_thread(){
    
    incomming = TinyWireS.available();

    if(incomming >=4){
        digitalWrite(led, HIGH); 
        i2c_data[0] = TinyWireS.receive();
        i2c_data[1] = TinyWireS.receive();
        i2c_data[2] = TinyWireS.receive();
        i2c_data[3] = TinyWireS.receive();
        if(i2c_data[0] == CMD_WRITE_SERVO_ANGLE){
            // Trans unsigned char to unisnged int
            data_u8_u16.u8[0] = i2c_data[2];
            data_u8_u16.u8[1] = i2c_data[3];
            // Write servo angle
            Servo_Out[i2c_data[1]].write(data_u8_u16.u16); 
        }
        else if(i2c_data[0] == CMD_SET_SERVO_MIN_PULSE){
            // Trans unsigned char to unisnged int
            data_u8_u16.u8[0] = i2c_data[2];
            data_u8_u16.u8[1] = i2c_data[3];
            // Write minimum pulse
            Servo_Out[i2c_data[1]].setMinimumPulse(data_u8_u16.u16); 
        }
        else if(i2c_data[0] == CMD_SET_SERVO_MAX_PULSE){
            // Trans unsigned char to unisnged int
            data_u8_u16.u8[0] = i2c_data[2];
            data_u8_u16.u8[1] = i2c_data[3];
            // Write maximum pulse
            Servo_Out[i2c_data[1]].setMaximumPulse(data_u8_u16.u16); 
        }
        digitalWrite(led, LOW); 
    }
}

void loop() 
{
    i2c_cmd_thread();
    SoftwareServo::refresh();
} 

