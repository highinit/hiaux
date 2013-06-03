sublist_t *noden;
recv(socket,(*noden).name , 255, 0); 


recv(socket, bf , 255, 0); 

sscanf(bf, "%d ", &(*noden).b ); 


recv(socket, bf, 255, 0); 
(*noden).(*hui ).resize(charptoint(bf)); 
for (int i1 = 0; i1<(*noden).(*hui ).size(); i1++)
{ 
		recv(socket, bf , 255, 0); 

		sscanf(bf, "%f ", &(*noden).((*hui )[i1]).x ); 

} 

vector<mlist_t> *files;

recv(socket, bf, 255, 0); 
(*files).resize(charptoint(bf)); 
for (int i0 = 0; i0<(*files).size(); i0++)
{ 

		recv(socket, bf, 255, 0); 
	((*files)[i0]).a .resize(charptoint(bf)); 
	for (int i2 = 0; i2<((*files)[i0]).a .size(); i2++)
	{ 
			recv(socket,((*files)[i0]).a [i2].name , 255, 0); 


			recv(socket, bf , 255, 0); 

			sscanf(bf, "%d ", &((*files)[i0]).a [i2].b ); 


						recv(socket, bf, 255, 0); 
			((*files)[i0]).a [i2].(*hui ).resize(charptoint(bf)); 
			for (int i4 = 0; i4<((*files)[i0]).a [i2].(*hui ).size(); i4++)
			{ 
					recv(socket, bf , 255, 0); 

					sscanf(bf, "%f ", &((*files)[i0]).a [i2].((*hui )[i4]).x ); 

			} 

	} 


		recv(socket, bf, 255, 0); 
	((*files)[i0]).b .resize(charptoint(bf)); 
	for (int i2 = 0; i2<((*files)[i0]).b .size(); i2++)
	{ 
		recv(socket,((*files)[i0]).b [i2], 255, 0); 

	} 

} 

int node;
recv(socket, bf , 255, 0); 

sscanf(bf, "%d ", &node); 
char *file;

