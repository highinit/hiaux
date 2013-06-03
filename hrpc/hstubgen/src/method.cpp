#include "incl.h"
#include "method.h"

#include "commonstub.h"
//#include "typet.h"
/*using namespace std;

extern bool stubs;
extern int tabs;

extern int subi;
extern ofstream fout;*/

	void method_t::ctype(const char *tname,const char *vname)
	{
/*		for (int i = 0; i<type->size(); i++)
		if (!strcmp((*type)[i]->name, tname))
		{
			if (!stubs)
			(*type)[i]->cstub((char*)"" ,vname, NULL, 0, STUB, 0);
			else
			(*type)[i]->cstub((char*)"" ,vname, NULL, 0, SKEL, 0);

		}
*/	}

	void method_t::gen_send_method(FILE *out, char *fgclass)
	{
		for (int i = 0; i<params.size(); i++)
		{

			puttabs(tabs);
			type_t *curobj = type->at(0)->gettype(nop(params[i]));
			if (isp(params[i]))
				cout << "objrepr_send(" << gen_func_by_type(nop(params[i]), fgclass)  << "(" <<names[i] <<"), ch, 1);\n";
			else
				cout << "objrepr_send(" << gen_func_by_type(nop(params[i]), fgclass)  << "(&" <<names[i] <<"), ch, 1);\n";
		}

		puttabs(tabs);
		cout << ret << " *ret;\n";
		puttabs(tabs);
		cout << "ret = " << neg_func_by_type(ret, fgclass) <<"(objrepr_recv(ch, 1));\n";
	}

	void method_t::gen_recv_method(FILE *out, char *fgclass)
	{
	    // recv params
		for (int i = 0; i<params.size(); i++)
		{
			puttabs(tabs);
			type_t *curobj = type->at(0)->gettype(nop(params[i]));
			if (isp(params[i]))
			cout << params[i] << " " << names[i] << " = "  \
				<< neg_func_by_type(nop(params[i]), fgclass) <<"(objrepr_recv(" << "ch, 1));\n";
			else
			cout << params[i] << " *" << names[i] << " = "  \
				<< neg_func_by_type(nop(params[i]), fgclass) <<"(objrepr_recv(" << "ch, 1));\n";
		}
		puttabs(tabs);

		// call method
		if (!instring("void", ret))
		{
			if (!retpointer && strcmp(ret, "char"))
			{
				cout << ret << " *ret = new " << ret << ";\n";
				puttabs(tabs);
				cout  << "*ret = obj->" << name << "(";
			}
			else
			{
				cout << ret << " *ret;\n";
				puttabs(tabs);
				cout << "ret = obj->" << name << "(";
			}
		}
		else cout << "obj->" << name << "(";
		for (int i = 0; i<params.size(); i++)
		{
			if (isp(params[i]))
			cout << " " << nop(names[i]);
			else
			cout << " *" << nop(names[i]);
			if (i!=params.size()-1) cout << ",";
		}

		cout <<");\n";
		puttabs(tabs);

        // !!!!!!!!!! TODO: clean memory
        for (int i = 0; i<params.size(); i++)
        if (isp(params[i]))
        {
            type_t *curobj = type->at(0)->gettype(nop(params[i]));
            if (curobj->vec)
            {

            }
            else
            {/*
                if (!strcmp(curobj->name, "char") || !strcmp(curobj->name, "char*"))
                {
                    cout << "delete [] " << nop(names[i]) << ";\n";
                }*/
            }
        }
        //puttabs(tabs);

		// sendback ret
		cout << "objrepr_send(" << gen_func_by_type(ret, fgclass) << "(ret), ch, 1);\n";
       /* puttabs(tabs);
        if (!strcmp(nop(ret), "char"))
            cout << "delete [] ret;\n";
        else
            cout << "delete ret;\n";*/
	}
