//Functions
void printString(char* string);
void readString(char* string);
void readSector(char* buffer, int sector);
int mod(int a, int b);
int div(int a, int b);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readFile(char * fileName, char * buffer);
int nameChecker(char * fName,char * dirName, int count);

int main() {

	char buffer[13312];
	printString("Testing\0");
	makeInterrupt21();
	interrupt(0x21, 3, "messag\0", buffer, 0); 
	interrupt(0x21, 0, buffer, 0, 0);
	while(1){

	}
}

void printString(char* string){ 
	//print strings out the screen
	int position = 0;
	while(string[position] != '\0'){
		interrupt(0x10, 0xE*256+string[position], 0, 0, 0);
		position = position +1;
	}
}
void readString(char* string){
	//read strings from a user
	int position = 0;
	char character = 0x0;
	while(character != 0xd){
		character = interrupt(0x16, 0, 0, 0, 0);
		if(character == 0xd){
			break;
		}
		else if(character == 0x8){
			if(position > 1){
				position = position -1;
				interrupt(0x10, 0xE*256+character, 0, 0, 0);
			}
		}
		else {
			string[position] = character;
			position = position +1;
			interrupt(0x10, 0xE*256+character, 0, 0, 0);
		}
	}
	interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
	string[position] = 0xa;
	string[position+1]= 0x0;
}
void readSector(char* buffer, int sector){
	//read from a sector
	int relSec = mod(sector,18)+1;
	int head= mod(div(sector,18),2);
	int track = div(sector,36);
	interrupt(0x13, 2*256+1, buffer, track*256+relSec, head*256+0);
	
}

int mod(int a, int b){
	//simple mod function to get the remainder
	while ( a >= b){
		a = a-b;
	}
	return a;

}

int div(int a, int b){
	//simple function for division
	int q = 0;
	while (q*b <= a){
		q = q+1;
	}
	return q-1;
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	//handles the interrupt I made
	int *bxp = bx;
	
	if(ax == 0){
		//this is for the printing strings
		while( *bxp != '\0'){
			interrupt(0x10, 0xE*256+*(bxp), 0, 0, 0);
			bxp= bxp+1;
		}
	} else if (ax == 1){
		//this is for reading strings
		int base =bx;
		char character = 0x0;
		while(character != 0xd){
			character = interrupt(0x16, 0, 0, 0, 0);
			if(character == 0xd){
				break;
			}
			else if(character == 0x8){
				if(bxp > base){
					bxp = bxp -1;
					interrupt(0x10, 0xE*256+character, 0, 0, 0);
				}
			}
			else {
				*(bxp) = character;
				bxp = bxp +1;
				interrupt(0x10, 0xE*256+character, 0, 0, 0);
			}
		}
		interrupt(0x10, 0xE*256+0xa, 0, 0, 0);
		*(bxp) = 0xa;
		bxp = bxp+1;
		*(bxp) = 0x0;

	} else if (ax == 2){
		//reading from a sector
		interrupt(0x13, ax, bx, cx, 0);
	} else if (ax == 3){
		//reads a file into a buffer
		char* fileName = bx;
		char * buffer = cx;
		readFile(fileName, buffer);
	} else {
		printString("Error invalid input was entered");
	}
}
void readFile(char* fileName, char* buffer){
	//reads a file into a buffer
	char sector[512];
	int sec = 2;
	int currBuffer = 0;
	int x = 0;
	int currFile = 0;
	int foundFile = 0;
	readSector(sector, sec);
	while (currFile < 512) {
		foundFile = nameChecker(fileName, sector+currFile,1);
		if(foundFile = 1){
			currFile = currFile+6;
			break;
		} else {
			currFile = currFile+32;
		}
	}	
	if (foundFile == 0){
		printString("No File \0");
		return; //file not found
	}

	while(x < 26){
		if(sector[currFile] != 0x0){
		readSector(buffer+currBuffer,sector[currFile]);
		currFile = currFile+1;
		currBuffer = currBuffer + 512;	
		x = x +1;
		}
	}	
}

int nameChecker(char * fName,char * dirName, int count){
	if(count == 6){
	 printString("here 1 here\0");
	 return 1;
	} else if ( fName[0] == dirName[0]) {
		printString("here here\0");
		return nameChecker(fName[1], dirName[1], count +1);
	} else {
		return 0;
	}
}






