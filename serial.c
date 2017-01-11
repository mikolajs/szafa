int readCredits(){
    struct sp_port *port;
    sp_return error = sp_get_port_by_name("ttyUSB0",&port);
    char c = '\0';
    if (error == SP_OK) {
      sp_set_baudrate(port,57600);
      error = sp_open(port,SP_MODE_READ);
      sleep(0.5);
      int bytes_waiting = sp_input_waiting(port);
        if (bytes_waiting > 0) {
          printf("Bytes waiting %i\n", bytes_waiting);
          char byte_buff[512];
          int byte_num = 0;
          byte_num = sp_nonblocking_read(port,byte_buff,512);
          if(byte_num > 0) c =  byte_buff[0];
        }
    }
    return (int) c;
}

int readCredits(){
    std::ifstream plik;
    char c;
      ///dev/ttUSB0
    plik.open("/dev/ttACM0");
    plik >> c;
    plik.close();
    return (int) c;
}

void askCredits(){
    std::ofstream plik;
      ///dev/ttUSB0
    plik.open("/dev/ttACM0");
    plik << "i\n";
    plik.close();
}
