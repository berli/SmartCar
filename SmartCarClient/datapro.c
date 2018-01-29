

void dataprocs()
{
	unsigned int program[0x3c,9]={0};
	if(values[0] <= 0x3C)
	{
			if (values.Length == values[2] + 5)   
			{
					if (values[1] == 3)
					{
								Program[values[0]-1][0] = values[15] * 256 + values[16];
								Program[values[0]-1][1] = values[17] * 256 + values[18];
								Program[values[0]-1][2] = values[19] * 256 + values[20];
								Program[values[0]-1][3] = values[3] * 256 + values[4];
								Program[values[0]-1][4] = values[5] * 256 + values[6];
								Program[values[0]-1][5] = values[7] * 256 + values[8];
								Program[values[0]-1][6] = values[27] * 256 + values[28];
								Program[values[0]-1][7] = values[35] * 256 + values[36];
								Program[values[0]-1][8] = values[45] * 256 + values[46];
					}
			}
	}
}