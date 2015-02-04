#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

gint sort_strcmp(guint *a,guint *b);
gint sort_intcmp(gconstpointer a,gconstpointer b);

int main(int argc, char *argv[]){

	GDir *dir = NULL;
	const gchar *fname = NULL;
	const gchar *output = "optext";
	gchar *ctemp;
	GString *word = g_string_new(NULL);
	char c;
	FILE *file;
	GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
	GSList *list = NULL;
	GPtrArray *dict = g_ptr_array_new();
	gint i = 0;

	if(argc>1){
		dir = g_dir_open(argv[1],0,NULL); // Open Directory 
		g_chdir(argv[1]); // change dir to /data to open file
		while((fname = g_dir_read_name(dir))!=NULL){ // Get Filename at random
			file = fopen(fname,"r");

			g_string_append(word,fname);
			g_string_erase(word,0,4);
			g_string_truncate(word,(word->len)-4);
			fname = g_strdup(word->str);
			g_string_erase(word,0,-1);


			if(!file) {
				printf("Open Error\n");
				return 1;
			}
			do{
				c = fgetc(file);
				if(isalpha(c)){
					g_string_append_c(word,tolower(c)); 
				}else if(word->len >0){
					if((list = g_hash_table_lookup(hash,word->str)) == NULL){
						list = g_slist_prepend(list,g_strdup(fname));
						ctemp = g_strdup(word->str); // New index of word prevent changing string
						g_hash_table_insert(hash,ctemp,list);
						g_ptr_array_add(dict,ctemp);
						g_string_erase(word,0,-1);	
						i++;
					}else{
						if(g_strcmp0(fname,list->data)!=0) list = g_slist_prepend(list,g_strdup(fname));
						g_hash_table_insert(hash,word->str,list);
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
	fprintf(file, "%d\n",dict->len);
	for( i = 0;i < (dict->len);i++){
		ctemp = g_ptr_array_index(dict,i);
		list = g_hash_table_lookup(hash,ctemp);
		list = g_slist_sort(list,(GCompareFunc)sort_intcmp);
		fprintf(file,"%s:%d:",ctemp,g_slist_length(list));
		while(g_slist_next(list) != NULL){
			fprintf(file,"%s,",list->data);
			list = g_slist_next(list);
		}
		fprintf(file,"%s\n",list->data);
		g_slist_free (list);
	}
	g_hash_table_destroy (hash);
	fclose(file);

	return 0;
}

gint sort_strcmp(guint *a,guint *b){
	return g_strcmp0(GUINT_TO_POINTER(*a),GUINT_TO_POINTER(*b));
}
gint sort_intcmp(gconstpointer a,gconstpointer b){
	int c = 0,d = 0;
	c = atoi(a);
	d = atoi(b);
	if(c<d)return -1;
	else if(c==d) return 0;
	else return 1;
}
