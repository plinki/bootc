# hi

mbr needs
- boot signature at the end of the sector (should be 0x55AA)
- boot code (the first 446 bytes of the sector)
- disk signature (if present)
- reserved fields in the MBR partition table

