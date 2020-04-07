#include <iostream>
#include <fstream>
#include <cstdint>

// arr must be of size 4
uint32_t array_to_int(unsigned char* arr)
{
   uint32_t result = 0;

   
   for (int i = 3; i >= 0; i--)
   {
      uint32_t addition = ((uint32_t)arr[i]) << 8 * (3-i);
      result += addition;
   }

   return result;
}

int main(int argc, char** argv)
{
   if (argc == 1)
      std::cout << "Must enter filename as first argument." << std::endl;
   else if (argc > 2)
      std::cout << "Too many arguments to script" << std::endl;
   else
   {
      // filename passed by user now pointed to by argv[1]
      std::ifstream rf(argv[1], std::ios::out | std::ios::binary);

      if (!rf)
      {
         std::cout << "Could not open file!" << std::endl;
         return 1;
      }

      unsigned char png_signature[] {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a}; // all png files must begin with these 8 bytes
      unsigned char png_check[8]; // we'll fill this with the first 8 bytes of the file

      rf.read((char*)png_check, 8);

      for (int i = 0; i < 8; i++)
      {
         if (png_signature[i] != png_check[i])
         {
            std::cout << "File " << argv[1] << "not a png. Exiting" << std::endl;
            return 1;
         }
      }

      unsigned char buff[128];

      while (!rf.eof())
      {
         uint32_t chunk_length;
         uint32_t chunk_crc;

         // read chunk size
         rf.read((char*)buff, 4);
         buff[4] = '\0';

         chunk_length = array_to_int(buff);

         // read chunk type
         rf.read((char*)buff, 4);
         buff[4] = '\0'; // so the string constructor doesn't look at the rest of the buffer unnecessarily

         std::string chunk_type((char*)buff);

         // read chunk data
         int bytes_left_in_data = chunk_length;
         while (bytes_left_in_data > 0)
         {
            if (bytes_left_in_data < 128)
            {
               rf.read((char*)buff, bytes_left_in_data);
               break;
            }
            rf.read((char*)buff, 128);
            bytes_left_in_data -= 128;
         }

         // read chunk crc
         rf.read((char*)buff, 4);
         buff[4] = '\0';

         chunk_crc = array_to_int(buff);
         


         std::cout << "chunk type: " << chunk_type << ", chunk length: " << std::dec << chunk_length << ", " << "chunk crc: " << chunk_crc << std::endl;

      }

   }
}
