
#include "typet.h"
#include "commonstub.h"

/*
extern int tabs;
extern bool stubs;

extern int subi;
extern ofstream fout;
*/
type_t::type_t(int t, const char *nm, vector<type_t*> *tp) 
	{ 
	type = tp; 
	basetype = t;  
	strcpy(name, nm); 
	vec = 0; 
	}
void type_t::addfield(const char *fl, const char *nm) 
	{ 
	char *bf1 = new char [255];
	char *bf2 = new char [255];
	strcpy(bf1, fl); 
	strcpy(bf2, nm);
	fields.push_back(bf1); 
	fnames.push_back(bf2);
	}
bool type_t::ifp(char *b) 
	{ 
		for (int i = 0; i<ps.size(); i++) 
		if (!strcmp(ps[i], b)) 
			return 1; 
	return 0; 
	} 
type_t::type_t() 
	{
		basetype = -1; 
		vec = 0;  
	}
void type_t::cbasetype(const char *last, const char *vname1, int mode)
	{
	char *vname;
/*	strcpy(bf, last);
	if (strlen(last)!=0)
	strcat(bf, ".");
	strcat(bf,vname);*/
	
	vname =  addtolast(last, vname1);
	
	
	puttabs(tabs);
	// send char *
	if (!strcmp(name, "char"))
	{
		if (mode==STUB)
		{
			cout << "strcpy(bf, " << nop(vname) << ");\n";
			puttabs(tabs);
			cout << "strcat(bf, \"\\n\");\n";
			puttabs(tabs);
			cout << "send(socket, " << "bf" << ", 255, 0); \n" << endl; 
		}
		else if (mode==TOJS)
		{
			if  (vname1[1]=='0')
				{
					puttabs(tabs);
					cout << "sprintf(bf, \" \\\" %s\\\", \" "  << ", " << vname << ");\n";
					puttabs(tabs);
					cout << "lines.push_back(bf); \n";
					puttabs(tabs);
					cout << "bf = new char [255];\n";
				}
			else 
				{
					puttabs(tabs);
					cout << "sprintf(bf, \" \\\""<< vname1 <<"\\\" : \\\" %s\\\", \" "  << ", " << vname << ");\n";
					puttabs(tabs);
					cout << "lines.push_back(bf); \n";
					puttabs(tabs);
					cout << "bf = new char [255];\n";
				}
		}
		else if (mode == SKEL)
			cout << "recv(socket," <<  nop(vname) << ", 255, 0); \n" << endl;
		else if (mode == FROMJS)
		{
			cout << "strcpy(bf, (*val)[0]);" << endl;
			puttabs(tabs);
			cout << "val->erase(val->begin());\n";
			
			puttabs(tabs);
			cout << "sscanf(bf, \"%s \", "<< vname << "); \n"; 	
		}
	}
	else if (!strcmp(name, "char*"))
	{
		if (mode==STUB)
		{
			cout << "strcpy(bf, " << nop(vname) << ");\n";
			puttabs(tabs);
			cout << "strcat(bf, \"\\n\");\n";
			puttabs(tabs);
			cout << "send(socket, " << "bf" << ", 255, 0); \n" << endl; 
		}
		else if (mode==TOJS)
		{
			puttabs(tabs);
			cout << "sprintf(bf, \" \\\""<< vname1 <<"\\\" : \\\" %s\\\", \" "  << ", " << vname << ");\n";
			puttabs(tabs);
			cout << "lines.push_back(bf); \n";
			puttabs(tabs);
			cout << "bf = new char [255];\n";
		}
		else if (mode == SKEL)
			cout << "recv(socket," <<  nop(vname) << ", 255, 0); \n" << endl;
		else if (mode == FROMJS)
		{
			cout << "strcpy(bf, (*val)[0]);" << endl;
			puttabs(tabs);
			cout << "val->erase(val->begin());\n";
			
			puttabs(tabs);
			cout << "sscanf(bf, \"%s \", "<< vname << "); \n"; 	
		}
	}
	else if (!strcmp(name, "int"))
	{
		if (mode==STUB)
		{
			puttabs(tabs);
			cout << "sprintf(bf, \"%d\\n\", " << vname << ");\n";
			puttabs(tabs);
			cout << "send(socket, bf , 255, 0); \n" << endl; 	
		}
		else if (mode==TOJS)
			if  (vname1[0]=='[')
			{
				puttabs(tabs);
				cout << "sprintf(bf, \"  \\\" %d\\\", \" "  << ", " << vname << ");\n";
				puttabs(tabs);
				cout << "lines.push_back(bf); \n";
				puttabs(tabs);
				cout << "bf = new char [255];\n";
			}
			else
			{
				puttabs(tabs);
				cout << "sprintf(bf, \" \\\""<< vname1 <<"\\\" : \\\" %d\\\", \" "  << ", " << vname << ");\n";
				puttabs(tabs);
				cout << "lines.push_back(bf); \n";
				puttabs(tabs);
				cout << "bf = new char [255];\n";				
			}
		else  if (mode == SKEL)
		{
			cout << "recv(socket, bf , 255, 0); \n" << endl;
			puttabs(tabs);
			cout << "sscanf(bf, \"%d \", &"<< vname << "); \n"; 
		}	
		else if (mode == FROMJS)
		{
			cout << "strcpy(bf, (*val)[0]);" << endl;
			puttabs(tabs);
			cout << "val->erase(val->begin());\n";
			
			puttabs(tabs);
			cout << "sscanf(bf, \"%d \", &"<< vname << "); \n"; 	
		}
	}
	else if (!strcmp(name, "float"))
	{
		if (mode==STUB)
			cout << "send(socket, " << "floattocharp(" << vname << ") , 255, 0); \n" << endl; 	
		else if (mode==TOJS)
		{
			if  (vname1[0]=='[')
			{
				puttabs(tabs);
				cout << "sprintf(bf, \"  \\\" %f\\\", \" "  << ", " << vname << ");\n";
				puttabs(tabs);
				cout << "lines.push_back(bf); \n";
				puttabs(tabs);
				cout << "bf = new char [255];\n";
			}
			else
			{
				puttabs(tabs);
				cout << "sprintf(bf, \" \\\""<< vname1 <<"\\\" : \\\" %d\\\", \" "  << ", " << vname << ");\n";
				puttabs(tabs);
				cout << "lines.push_back(bf); \n";
				puttabs(tabs);
				cout << "bf = new char [255];\n";				
			}
		}
		else  if (mode == SKEL)
		{
			cout << "recv(socket, bf , 255, 0); \n" << endl;
			puttabs(tabs);
			cout << "sscanf(bf, \"%f \", &"<< vname << "); \n"; 
		}		
		else if (mode == FROMJS)
		{
			cout << "strcpy(bf, (*val)[0]);" << endl;
			puttabs(tabs);
			cout << "val->erase(val->begin());\n";
			puttabs(tabs);
			cout << "sscanf(bf, \"%f \", &"<< vname << "); \n"; 
		}
	}
}

// '*' and ' '
void type_t::cstub(const char *last1, const char *vname1, FILE *out, int subi, int mode, bool inarr)
{
//printf("%s %d\n", name, mode);
	// variable name
	char *vname = new char [255];
	char *last = new char [255];
	strcpy(vname, vname1);
	strcpy(last, last1);
	
	
	if (isp(vname) || !strcmp("char", name)) 
	{
	if ((mode==FROMJS))// || (mode==SKEL))
		{
			if (strlen(last)>0) cout << last << ".";
			if (!strcmp("char", name))
			cout <<  nop(vname) << " = new char [255] ;\n";
			else
			cout <<  nop(vname) << " = new " << name << ";\n";	
		}		
	
	if (strcmp("char", name))
	last = addptolast(last, vname);
	else last = addtolast(last, vname);
	//cout << "HUIL: " << last << endl;
	}
	else last = addtolast(last, vname);
	//	cout << last << " " << " "<< name << " " << vname << "HUI\n";;
	//printf("%s \n", name);		
		if (vec)
		{
			if (mode==STUB)
			{
				puttabs(tabs);
				cout << "sprintf(bf, \"%d \\n\", " << last << ".size() );\n";
				puttabs(tabs);
				cout << "send(socket, bf, 255, 0); \n";
			}
			else if (mode==TOJS)
			{
				/////////
				puttabs(tabs);
				cout << "strcpy(bf, \" \\\"" << vname1 << "\\\": \"); \n";
				puttabs(tabs);
				cout << "lines.push_back(bf); \n";
				puttabs(tabs);
				cout << "bf = new char [255];\n";
				puttabs(tabs);
			
				cout << "sprintf(bf, \" [  \"); \n";
				puttabs(tabs);
				cout << "lines.push_back(bf); \n";
				puttabs(tabs);
			 	cout << "bf = new char [255];\n";
			}
			else if (mode==SKEL)
			{
				puttabs(tabs);
				cout << "recv(socket, bf, 255, 0); \n";
				puttabs(tabs);
				cout << "sscanf(bf, \"%d\", &bfi);\n";
				puttabs(tabs);
				if (!firststr("char", invec()))
				cout << last << ".resize(bfi, *(new "<< invec() <<")); \n";
				else 
				cout << last << ".resize(bfi, (new char [255])); \n";
			}
			else if (mode==FROMJS)
			{
			//	puttabs(tabs);
			//	cout << "recv(socket, bf, 255, 0); \n";
				puttabs(tabs);
				cout << "strcpy(bf, (*val)[0]);" << endl;
				puttabs(tabs);
				cout << "val->erase(val->begin());\n";
				puttabs(tabs);
				cout << "sscanf(bf, \"%d \", &bfi); \n"; puttabs(tabs);
				if (!firststr("char", invec()))
				cout << last << ".resize(bfi, *(new "<< invec() <<")); \n";
				else 
				cout << last << ".resize(bfi, (new char [255])); \n";
				
			}
			
			puttabs(tabs);
			cout << "for (int i"<<subi  <<" = 0; i"<< subi<<"<" << last << ".size(); i"<<subi <<"++)" << endl;
			puttabs(tabs);
			cout << "{ \n";
			tabs++;
			
			char *qut = new char [255];
			sprintf(qut, "[i%d]", subi);	
					
			ctype( last ,invec(), qut, subi+1, mode, 1);
			tabs--;
			puttabs(tabs);
			cout << "} \n";
			if (mode==TOJS)
			{
				puttabs(tabs);
				cout << "lines[lines.size()-1][strlen(lines[lines.size()-1])-1] = ' '; \n";
				puttabs(tabs);
				cout << "sprintf(bf, \" ],  \"); \n";
				puttabs(tabs);
				cout << "lines.push_back(bf); \n";
				puttabs(tabs);
				cout << "bf = new char [255];\n";
			}
			return;
		}
		else if (basetype>=0) 
		{
			//printf("%s \n", name);
			cbasetype(last1, vname, mode);
			return; 
		}
		else
		{
		//printf("%s \n", name);
		// 60 63 65-0  0 0 52 63 50 53 55-0 52
		//printf("%d\n",fields.size());
			if (mode==TOJS)
			{
				if (!inarr)
				{
					cout << "sprintf(bf, \"\\\"" << vname << "\\\" : {\");\n";
					cout << "lines.push_back(bf); \n";
					cout << "bf = new char [255];\n";
				}
				else 
				{
					cout << "sprintf(bf, \" {\");\n";
					cout << "lines.push_back(bf); \n";
					cout << "bf = new char [255];\n";
				}
			}
			
			for (int i = 0; i<fields.size(); i++)
			{
				//printf("	%s %s\n", fields[i], fnames[i]);
				//cout <<"----------"<< last << "--"<<fields[i] << " " << fnames[i] << endl;
						
				ctype(last, fields[i], fnames[i], subi+1, mode, 0);
				
				//cout << endl;
			}
				if (mode==TOJS)
				{
				// delete ','
				cout << "lines[lines.size()-1][strlen(lines[lines.size()-1])-2] = ' '; \n";
				cout << "sprintf(bf, \"},\" );\n";
				cout << "lines.push_back(bf); \n";
				cout << "bf = new char [255];\n";
				}
			return;
		//cout << "none: " << vname << " " << basetype<<endl;	
		}	
}

void type_t::ctype(const char *last, const char *tname, const char *vname, int subi, int mode, bool inarr)
{
//	cout << last << " " << tname << " " << vname << endl;
	
	if (!strcmp(tname, "char*"))
	{
	(*type)[1]->cstub( last, vname, NULL, subi, mode, inarr);	
	return;	
	}
//		(*type)[2]->cstub( last, vname, NULL, subi, mode);	
//	else 
	for (int i = 0; i<type->size(); i++)
	if (!strcmp((*type)[i]->name, nop(tname)))// || !strcmp(nop((*type)[i]->name), nop(tname)))
	{
		(*type)[i]->cstub( last, vname, NULL, subi, mode, inarr);	
		return;
	}
}

/*
void type_t::jsontype(char *last, char *tname, char *vname, int subi)
{
	for (int i = 0; i<type->size(); i++)
	if (!strcmp((*type)[i]->name, tname))
	{
		(*type)[i]->cstub( last, vname, NULL, subi, 1);	
	}
}
*/