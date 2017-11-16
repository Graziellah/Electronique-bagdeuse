/* Code source tps://github.com/CytronTechnologies/DIY-PR25-PIC16F876A/blob/master/Source%20file/PR25.c*/

# define DATA0 PORTDbits.RD9
# define DATA1 PORTDbits.RD10
# define FLASHSPI register.flash

void delay(unsigned short i) {

  while(--i != 0);
}

void	check_database(unsigned char database)
{
	if (database == 1)
	{
		RELAY = 1;
		LED = 0;
		writeStringUART2("Id no:");
		//
		//
		writeStringUART2("User identified.");
		repeat = 12;
		while(repeat != 0)
		{
			delay(100000);
			repeat --;
		}
		lcd_clear();
		//
		//
		
	}
	else if (database == 2)
	{
		LED = 0;
		writeStringUART2("Id no:");
		//
		//
		writeStringUART2("User ID saved");
	}
	else
	{
		RELAY = 0;
		LED = 1;
		writeStringUART2("Id no:");
		//
		//
		writeStringUART2("User no found");
	}
}

/* reader mode

Mode = 0 the reader is waiting, no RFID tag is placed on the RFID
Mode = 1 Tag is placed on the RFID reader(reader receive data stream from the RFID tag)
Mode = 2 Save mode
Mode = 3 Delete mode
*/

void main(void) 
{
  unsigned char   i, repeat, b, database;
  unsigned char   data[26];
  unsigned char   convert1 = 0;
  unsigned char   convert2 = 0;
  unsigned char   mode = 0;               //clear the mode
  unsigned char   id1(2) = (127);         //init 1st id1 in the array
  unsigned int    id2(2) = (22449);      //init 1st id2 in the array

  init();                                 //Initialisation
  lcd_clear();
  id1[1] = 0;                             //clear secind id1 int hre array, first id1 is Initialisation
  id2[1] = 0;
  ici_init_light();
  writeStringUART2("Hello please place your tag on the reader");

  while(1)
  {
    convert1 = 0;
    convert2 = 0;
    lcd_clear();
    writeStringUART2("Place your tag on the reader");
    while(mode = 0)                       //no RFID tag is placed on RFID Reader
    {
		if ((DATA0 == 0) || (DATA1))        //RFID tag is placed on RFID reader(receive date stream from RFID tag)
			mode = 1;
		else if(BTN = 1)                    // If button is pressed once
			mode = 2;
		else if (BTN = 2)					// If button is pressed twice
			mode = 3;
	}
    // RFID tag is placed on RFID reader(RFID reader receive data stream from RFID tag)
    while(mode == 1)
    {
      while (i < 26)                        // repeat the loop until all 26 bit date are sent (start from 0 to 25)
      {
        while((DATA0 == 1) && (DATA1 == 0)); // wait while data0 and date1 remain at hight logic level( no changes at data0 and data1)
        if((DATA0 == 0) && (DATA1 == 1))
        {
            date[i] = 0;
            while((DATA0 == 0) && (DATA1 == 1)); //wait for the data stream finish sending from RFID tag again
        }
        if((DATA0 == 1) && (DATA1 == 0))
        {
            date[i] = 1;
            while((DATA0 == 1) && (DATA1 == 0)); //wait for the data stream finish sending from RFID tag again
        }
        i++;
      }
	  mode = 1;
	  i = 0;									//Clear i
	  lcd_clr();
	  while(i < 8)								//Loop for data[0]-data[7]
	  {
		  convert1=(convert1<<1)|data[i + 1];	//Shift current date and combine with previous data in convert1
		  i++;
	  }
	  i = 0;
	  while(i < 16)								//Loop for data[0]-data[7]
	  {
		  convert2=(convert2<<1)|data[i + 9];	//Shift current date and combine with previous data in convert2
		  i++;
	  }
	  b = 0;
	  while(b < 2)								//Compare id[0] ans id[1]	
	  {
		  if((convert1== id1[b]) && (convert2 == id2[b])) // id doesn't match, set database to 1
			  database = 1;
		  b++;
	  }
	  // check data with memory flash
	  lcd_clr();	  
    }
	// push button is pressed, enter to save mode
	while(mode == 2)
	{
		lcd_clear();
		writeStringUART2("Save mode");
		writeStringUART2("Place your tag");
		while (i < 26)                        // repeat the loop until all 26 bit date are sent (start from 0 to 25)
		{
			while((DATA0 == 1) && (DATA1 == 0)); //wait for data0 and data1 to change
			if((DATA0 == 0) && (DATA1 == 1))	// if data0 change(data0 is active low)
			{
				date[i] = 0;						// save that the bit received is 0
				while((DATA0 == 0) && (DATA1 == 1)); //wait until data0 and data1 back to hight logic
			}
			else if((DATA0 == 1) && (DATA1 == 0))	//if data1 chages (data1 is active low)
			{
				date[i] = 1;						//save that bit received is 1
				while((DATA0 == 1) && (DATA1 == 0)); //wait until data0 and data1 back to hight logic
			}
			i++;
		}
		mode = 0;
		i = 0;
		  lcd_clr();
		while(i < 8)								//Loop for data[0]-data[7]
		{
			convert1=(convert1<<1)|data[i + 1];	//Shift current date and combine with previous data in convert1
			i++;
		}
		i = 0;
		while(i < 16)								//Loop for data[0]-data[7]
		{
			convert2=(convert2<<1)|data[i + 9];	//Shift current date and combine with previous data in convert2
			i++;
		}
		id1[1] = convert1;
		id2[1] = convert2;
		// Envoyer data to memory flash
		database = 2;
		lcd_clr();	
	}
	while (mode == 3)
	{
		writeStringUART2("Delete mode");
		writeStringUART2("Place your tag");
		
		
	}
	check_database(database);
	repeat = 12;
	while(repeat != 0)
	{
		delay(100000);
		repeat --;
	}
	i = 0;
	LED = 0;
	database =0;
	convert1 = 0;
	convert2 = 0;
	}
  }
}
/*
 CONTROL REGISTERS
The Interrupts module consists of the following 
Special Function Registers (SFRs): 
-INTCON: Interrupt Control Register
-INTSTAT: Interrupt Status Register
-IPTMR: Interrupt Proximity Timer Register
-IFSx: Interrupt Flag Status Register. Say if the interrupts has occurred
-IECx: Interrupt Enable Control Register
-IPCx: Interrupt Priority Control Register
*/
void	_ISR(_EXTERNAL_1_VECTOR, IPL1AUTO) Savemode()
{

}

