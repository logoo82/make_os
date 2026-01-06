void Main()
{
    int line = 5;
    char str[9] = "C Kernel!";
    char* video = (char*)(0xB8000 + 160*line);

    for(int i = 0; str[i] != 0; i++)
    {   
        *video++ = str[i];
        *video++ = 0x0A;
    }

    while(1);

}