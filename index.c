#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

gint sort_strcmp(guint *a,guint *b);
gint sort_intcmp(guint *a,guint *b);

int main(int argc, char *argv[]){

	GDir *dir = NULL;
	const gchar *fname = NULL;
	const gchar *output = "optext";
	gchar *ctemp,*ftemp;
	GString *word = g_string_new(NULL);
	char c;
	FILE *file;
	GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
	GPtrArray *dict = g_ptr_array_new();
	GPtrArray *farray;
	gint i = 0,j=0;

	if(argc>1){
		dir = g_dir_open(argv[1],0,NULL); // Open Directory 
		g_chdir(argv[1]); // change dir to /data to open file
		while((fname = g_dir_read_name(dir))!=NULL){ // Get Filename at random
			file = fopen(fname,"r");

			for(i=0;fname[i]!='\0';i++){
				if(isdigit(fname[i])) g_string_append_c(word,fname[i]); 
			}
			fname = g_strdup(word->str);
			ftemp = g_strdup(fname);
			g_string_erase(word,0,-1);


			if(!file) {
				printf("Open Error\n");
				exit(1);
			}
			do{
				c = fgetc(file);
				if(isalpha(c)){
					g_string_append_c(word,tolower(c)); 
				}else if(word->len >0){
					if((farray = g_hash_table_lookup(hash,word->str)) == NULL){
						farray = g_ptr_array_new();
						g_ptr_array_add(farray,ftemp);
						ctemp = g_strdup(word->str); // New index of word prevent changing string
						g_hash_table_insert(hash,ctemp,farray);
						g_ptr_array_add(dict,ctemp);
						g_string_erase(word,0,-1);	
					}else{
						if(g_strcmp0(fname,g_ptr_array_index(farray,farray->len-1))!=0){
							g_ptr_array_add(farray,ftemp);
						}
						g_hash_table_insert(hash,word->str,farray);
						g_string_erase(word,0,-1);	
					}
				}			
			} while(c != EOF);
			fclose(file);
		} 
		g_dir_close(dir);
	}

	g_ptr_array_sort(dict,(GCompareFunc)sort_strcmp);

	g_chdir("..");
	file = fopen(output,"w");
	fprintf(file,"%d\n",dict->len);
	for( i = 0;i < (dict->len);i++){
		ctemp = g_ptr_array_index(dict,i);
		farray = g_hash_table_lookup(hash,ctemp);
		g_ptr_array_sort(farray,(GCompareFunc)sort_intcmp);
		fprintf(file,"%s:%d:",ctemp,farray->len);
		for(j = 0;j<(farray->len-1);j++){
			fprintf(file,"%s,",g_ptr_array_index(farray,j));
		}
		fprintf(file,"%s\n",g_ptr_array_index(farray,farray->len-1));
		g_ptr_array_free(farray,TRUE);
	}
	g_hash_table_destroy (hash);
	fclose(file);

	return 0;
}

gint sort_strcmp(guint *a,guint *b){
	return g_strcmp0(GUINT_TO_POINTER(*a),GUINT_TO_POINTER(*b));
}
gint sort_intcmp(guint *a,guint *b){
	 return atoi(GUINT_TO_POINTER(*a)) - atoi(GUINT_TO_POINTER(*b));
}
