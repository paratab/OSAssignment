#include <stdio.h>
#include <glib.h>

int main(int argc, char *argv[]){

	GDir *dir = NULL;
	const gchar *fname = NULL;
	GString *word = g_string_new(NULL);
	char c;
	FILE *file;

	if(argc>1){
		dir = g_dir_open(argv[1],0,NULL);
		g_chdir(argv[1]); // change dir to /data to open file
		while((fname = g_dir_read_name(dir))!=NULL){
			//printf("Open : %s :",fname);
			file = fopen(fname,"r");
			if(!file) {
				//printf("Open Error\n");
				return 1;
			}
			//printf("Done\n");
			do{
				c = fgetc(file);
				if(isalpha(c)){
					g_string_append_c(word,tolower(c));
				}else{
					//Do String here
					//printf("%s ",word->str);
					g_string_assign (word,"");
				}				
			} while(c != EOF);
			fclose(file);
		} 
		g_dir_close(dir);
	}
	return 0;

}
