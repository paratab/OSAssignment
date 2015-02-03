#include <stdio.h>
#include <glib.h>

int main(int argc, char *argv[]){

	GDir *dir = NULL;
	const gchar *fname = NULL;
	GString *word = g_string_new(NULL);
	char c;
	FILE *file;
	GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);

	if(argc>1){
		dir = g_dir_open(argv[1],0,NULL); // Open Directory 
		g_chdir(argv[1]); // change dir to /data to open file
		while((fname = g_dir_read_name(dir))!=NULL){ // Get Filename at random
			file = fopen(fname,"r");
			if(!file) {
				printf("Open Error\n");
				return 1;
			}
			do{
				c = fgetc(file);
				if(isalpha(c)){
					g_string_append_c(word,tolower(c)); 
				}else if((word->len)>0){
					g_hash_table_insert(hash,word->str,(*fname));
					printf("%s from %s\n",word->str,g_hash_table_lookup(hash,word->str));
					g_string_assign(word,""); 
				}				
			} while(c != EOF);
			fclose(file);
		} 
		g_dir_close(dir);

	}
	return 0;
}
