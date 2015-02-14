#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

gint sort_strcmp(guint *a,guint *b);
gint sort_intcmp(gconstpointer a,gconstpointer b);

int main(int argc, char *argv[]){

	GDir *dir = NULL;
	const gchar *fname = NULL;
	const gchar *output = "optext";
	gchar *ctemp,*ftemp;
	GString *word = g_string_new(NULL);
	char c;
	FILE *file;
	GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);
	GSList *list = NULL;
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
			ftemp = g_strdup(fname);


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
						list = g_slist_prepend(list,ftemp);
						ctemp = g_strdup(word->str); // New index of word prevent changing string
						g_hash_table_insert(hash,ctemp,list);
					}else{
						if(g_strcmp0(fname,list->data)!=0) list = g_slist_prepend(list,ftemp);
						g_hash_table_insert(hash,word->str,list);
					}
					g_string_erase(word,0,-1);	
				}			
			} while(c != EOF);
			fclose(file);
		} 
		g_dir_close(dir);
	}

	g_chdir("..");
	file = fopen(output,"w");
	GList *hlist = g_hash_table_get_keys (hash);
	fprintf( file,"%d\n",g_list_length(hlist));

	GList *temp = hlist;
	hlist = g_list_sort(hlist,(GCompareFunc)g_strcmp0);
	temp = hlist;
	while(temp != NULL){
		list = g_hash_table_lookup(hash,temp->data);
		list = g_slist_sort(list,(GCompareFunc)sort_intcmp);
		fprintf( file,"%s:%d:",temp->data,g_slist_length(list));
		while(g_slist_next(list) != NULL){
			fprintf( file,"%s,",list->data);
			list = g_slist_next(list);
		}
		fprintf( file,"%s\n",list->data);
		g_slist_free (list);
		temp = g_list_next( temp);
	}
	g_hash_table_destroy (hash);
	fclose(file);

	return 0;
}

gint sort_strcmp(guint *a,guint *b){
	return g_strcmp0(GUINT_TO_POINTER(*a),GUINT_TO_POINTER(*b));
}
gint sort_intcmp(gconstpointer a,gconstpointer b){
	 return atoi(a) - atoi(b);
}
