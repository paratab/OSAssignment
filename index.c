#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

gint sort_strcmp(guint *a,guint *b);
gint sort_intcmp(guint *a,guint *b);

int main(int argc, char *argv[]){

	const gchar *output = "optext";
	gchar *ctemp,*ftemp,fname[20];
	GString *word = g_string_new(NULL);
	char c;
	FILE *file;
	GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
	GPtrArray *dict = g_ptr_array_new();
	GPtrArray *farray;
	gint i = 0,j=0,filenum = 1;
	int *num;

	if(argc>1){
		g_chdir(argv[1]); // change dir to /data to open file
		while(1){ // Get Filename at random
			num = g_new(int,1);
			*num = filenum ++;
			sprintf(fname,"file%d.txt",*num);
			file = fopen(fname,"r");

			if(!file) {
				break;
			}
			do{
				c = fgetc(file);
				if(isalpha(c)){
					g_string_append_c(word,tolower(c)); 
				}else if(word->len >0){
					if((farray = g_hash_table_lookup(hash,word->str)) == NULL){
						farray = g_ptr_array_new();
						g_ptr_array_add(farray,num);
						ctemp = g_strdup(word->str); // New index of word prevent changing string
						g_hash_table_insert(hash,ctemp,farray);
						g_ptr_array_add(dict,ctemp);
						g_string_erase(word,0,-1);	
					}else{
						if(sort_intcmp(num,g_ptr_array_index(farray,farray->len-1))!=0){
							g_ptr_array_add(farray,num);
						}
						g_hash_table_insert(hash,word->str,farray);
						g_string_erase(word,0,-1);	
					}
				}			
			} while(c != EOF);
			fclose(file);
		} 
	}

	g_ptr_array_sort(dict,(GCompareFunc)sort_strcmp);

	g_chdir("..");
	file = fopen(output,"w");
	fprintf(file,"%d\n",dict->len);
	for( i = 0;i < (dict->len);i++){
		ctemp = g_ptr_array_index(dict,i);
		farray = g_hash_table_lookup(hash,ctemp);
		fprintf(file,"%s:%d:",ctemp,farray->len);
		for(j = 0;j<(farray->len-1);j++){
			num = g_ptr_array_index(farray,j);
			fprintf(file,"%d,",*num);
		}
		num = g_ptr_array_index(farray,farray->len-1);
		fprintf(file,"%d\n",*num);
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
	 return  *a - *b;
}
