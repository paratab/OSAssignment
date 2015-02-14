#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <pthread.h>
#include <malloc.h>

#define NUM_THREADS 2

GHashTable *hash;
GPtrArray *dict;
GDir *dir = NULL;
pthread_mutex_t mutexsum;


gint sort_strcmp(long *a,long *b);
gint sort_intcmp(gconstpointer a,gconstpointer b);
void *ReadFileDir(void *threadid); 
GSList *list = NULL;

int main(int argc, char *argv[]){

	FILE *file;
	gchar *ctemp,*ftemp;
	void *status;
	const gchar *output = "optext";
		
	gint i = 0;

	pthread_t threads[NUM_THREADS];
	gint rc = 0;
	glong t;

	hash = g_hash_table_new(g_str_hash, g_str_equal);
	dict = g_ptr_array_new();
	pthread_mutex_init(&mutexsum, NULL);

	if(argc>1){
		dir = g_dir_open(argv[1],0,NULL); // Open Directory 
		g_chdir(argv[1]); // change dir to /data to open file

		mallopt(-8,1);
		
		for(t=0;t<NUM_THREADS;t++){
			//printf("Main Create threads %ld\n",t);
			rc = pthread_create(&threads[t],NULL,ReadFileDir,(void *)t);
			if(rc){
				printf("Error; return code form pthred_create() is %d\n",rc);
				exit(-1);
			}
		}
		for(i=0; i<NUM_THREADS; i++){
	  		pthread_join(threads[i], &status);
		}
		g_dir_close(dir);
		
	}

	g_ptr_array_sort(dict,(GCompareFunc)sort_strcmp);

	g_chdir("..");
	file = fopen(output,"w");
	fprintf(file, "%d\n",dict->len);
	//rintf("%d\n",dict->len);
	for( i = 0;i < (dict->len);i++){
		ctemp = g_ptr_array_index(dict,i);
		list = g_hash_table_lookup(hash,ctemp);
		list = g_slist_sort(list,(GCompareFunc)sort_intcmp);
		fprintf(file,"%s:%d:",ctemp,g_slist_length(list));
		//printf("%s:%d:",ctemp,g_slist_length(list));
		while(g_slist_next(list) != NULL){
			fprintf(file,"%s,",list->data);
			//printf("%s,",list->data);
			list = g_slist_next(list);
		}
		fprintf(file,"%s\n",list->data);
		//printf("%s\n",list->data);
		g_slist_free (list);
	}
	
	g_hash_table_destroy (hash);
	fclose(file);

	pthread_mutex_destroy(&mutexsum);
	return 0;
}

gint sort_strcmp(long *a,long *b){
	return g_strcmp0(GUINT_TO_POINTER(*a),GUINT_TO_POINTER(*b));
}
gint sort_intcmp(gconstpointer a,gconstpointer b){
	 return atoi(a) - atoi(b);
}

void *ReadFileDir(void *threadid){
	FILE *file;
	GString *word = g_string_new(NULL);
	gchar *ctemp,*ftemp;
	const gchar *fname = NULL;
	GPtrArray *temp;
	GSList *list = NULL;
	char c;
	gint i=0;
	gboolean find = FALSE;


	while((fname = g_dir_read_name(dir))!=NULL){ // Get Filename at random
		file = fopen(fname,"r");

		temp = g_ptr_array_new();	// temp array for ser
		//printf(">> Open %s from thread[%ld] <<\n",fname,(long)threadid);

		g_string_append(word,fname);
		g_string_erase(word,0,4);
		g_string_truncate(word,(word->len)-4);
		fname = g_strdup(word->str);
		g_string_erase(word,0,-1);
		ftemp = g_strdup(fname);
		printf(">> Open %s from thread[%ld] <<\n",fname,(long)threadid);


		if(!file) {
			printf("Open Error\n");
			exit(1);
		}
		do{
			c = fgetc(file);
			if(isalpha(c)){
				g_string_append_c(word,tolower(c)); 
			}else if(word->len >0){
				//lock
				//pthread_mutex_lock (&mutexsum);
				//printf("%s\n",word->str);

				if((list = g_hash_table_lookup(hash,word->str)) == NULL){
					list = g_slist_prepend(list,ftemp);
					ctemp = g_strdup(word->str); // New index of word prevent changing string

					pthread_mutex_lock (&mutexsum);
					g_hash_table_insert(hash,ctemp,list);	
					g_ptr_array_add(dict,ctemp);
					pthread_mutex_unlock (&mutexsum);

					g_ptr_array_add(temp,ctemp);				
				}else{
					
					for(i=0;i<(temp->len);i++){
						if(g_strcmp0(g_ptr_array_index(temp,i),word->str)==0) {
							find = TRUE;
							//printf("hit\n");
						}
						//printf("[%s],",g_ptr_array_index(temp,i));
						
					}
					//printf("\n");
					if (!find){
						//printf("\nFound in hash but not already add %d ",g_slist_length(list));
						list = g_slist_prepend(list,ftemp);
						//printf("Extend List " );
						//printf("L2 = %d\n",g_slist_length(list));
					}
					find = FALSE;

					pthread_mutex_lock (&mutexsum);
					g_hash_table_insert(hash,word->str,list);
					pthread_mutex_unlock (&mutexsum);

					g_ptr_array_add(temp,g_strdup(word->str));
				}
				//pthread_mutex_unlock (&mutexsum);
				g_string_erase(word,0,-1);	
				
				//unlock
			}			
		} while(c != EOF);
		fclose(file);
		g_ptr_array_free(temp,TRUE);						
	} 
	pthread_exit((void*) 0);

}