#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct kidsList
{
	int key;
	struct kidsList *next;
}kidsList;
typedef kidsList *KidsList;

typedef struct address
{
	char *name;
	struct address *next;
}address;
typedef address *Address;

typedef struct usersList
{
	int key;
	int server;
	struct usersList *next;
}usersList;
typedef usersList *UsersList;

typedef struct node     //nodul din ierarhia DNS
{
	int key; //indexul nodului curent
	int parrent_id; //id-ul nodului parinte
	KidsList v; //lista care retine copiii nodului
	Address adr; //lista care retine adresele rezolvabile de nod
	UsersList users; //lista utilizatorilor legati la nod
	int nr_adr; //nr de adrese rezolvabile implicit de catre un nod
	struct node *next; //adresa urmatorului nod
}node;
typedef node *Node;


void freeMem1(Address lst) 
{ 
   Address aux=lst; 
   Address aux1; 
   while (aux!=NULL)  
   { 
       aux1=aux->next; 
       free(aux->name); 
       free(aux);
       aux=aux1; 
   } 
   lst=NULL; 
} 

void freeMem3(KidsList lst) 
{ 
   KidsList aux=lst; 
   KidsList aux1; 
   while (aux!=NULL)  
   { 
       aux1=aux->next; 
       free(aux); 
       aux=aux1; 
   } 
   lst=NULL; 
} 

void freeMem2(UsersList lst) 
{ 
	UsersList aux=lst; 
   	UsersList aux1; 
   	while (aux!=NULL)  
   	{ 
       aux1=aux->next; 
       free(aux); 
       aux=aux1; 
   	} 
   	lst=NULL; 
} 

void freeMem(Node *lst) 
{ 
   Node aux=*lst; 
   Node aux1; 
   while (aux!=NULL)  
   { 
       	aux1=aux->next; 
       	freeMem1(aux->adr);
       	freeMem2(aux->users);
    	freeMem3(aux->v);
    	free(aux); 
    	aux=NULL;
    	aux=aux1; 
   } 
   *lst=NULL; 
} 

Node convertToNode(Node *rez,int key) //returneaza adresa nodului cu valoarea key din lista rez
{
	Node server=*rez;
	while(server!=NULL&&server->key!=key)
		server=server->next;
	return server;
}

Address addAddress(Address lst,char *adr) //adauga un nou element in lista de adrese
{
	Address nou=(address *)malloc(sizeof(address));
	nou->name=(char *)malloc(10*sizeof(char));
	strcpy(nou->name,adr);
	if(lst==NULL)
	{
		nou->next=NULL;
		lst=nou;
	}
	else
	{
		Address aux=lst;
		while(aux->next!=NULL)
			aux=aux->next;
		nou->next=NULL;
		aux->next=nou;
	}
	return lst;
}

void addUser(Node *tree,int server,int key) //adauga in lista de useri un user identificat prin id_ul key
{
	Node server_nod=convertToNode(tree,server); //server este nodul din arbore identificat prin indexul server
	UsersList nou=(usersList *)malloc(sizeof(usersList)); 
	nou->key=key;
	if(server_nod->users==NULL)
	{
		nou->next=NULL;
		server_nod->users=nou;
	}
	else
	{
		UsersList aux=server_nod->users;
		while(aux->next!=NULL)
			aux=aux->next;
		nou->next=NULL;
		aux->next=nou;
	}
}

KidsList addLast(KidsList lst,Node server) //adauga un element in lista de copii a nodului server
{
	KidsList nou=(kidsList *)malloc(sizeof(kidsList));
	nou->key=server->key;
	if(lst==NULL) //lista este initial vida
	{
		nou->next=NULL;
		lst=nou;
	}
	else
	{
		KidsList aux=lst; //aux parcurge nodurile listei pana la ultimel element al acesteia
		while(aux->next!=NULL)
			aux=aux->next;
		aux->next=nou;
		nou->next=NULL;
	}
	return lst; //returneaza lista de copii a nodului server, primit ca parametru
}

void createNode(int key,int id,Address *adr,Node *listofServers) //adauga in lista listofServers un nou nod
{
    Node newNode=(node *)calloc(1,sizeof(node));
    newNode->key = key;
    newNode->parrent_id=id; //id-ul nodului parinte
    newNode->adr=*adr; //adresele rezolvabile de catre nod
    if(*listofServers==NULL)
    {
    	newNode->next=NULL;
    	*listofServers=newNode;
    }
    else
    {
    	Node aux=*listofServers;
    	while(aux->next!=NULL)
    		aux=aux->next;
    	aux->next=newNode;
    	newNode->next=NULL;
    }
}

Node getParrent(Node *rez,int key) //intoarce adresa nodului cu valoarea key din arborele cu n noduri
{
	Node server=*rez;
	while(server!=NULL)
	{
		if(server!=NULL)
		{
			if(server->key==key)
				return server;
		}
		server=server->next;
	}
	return NULL;//cand nodul nu se gaseste in arbore 
}


void addKids(Node *rez) ///adauga fiecarui nod din lista rez copiii acestuia
{
	Node aux;
	Node server=*rez;
	while(server!=NULL)
	{
		aux=server;
		Node parrent=getParrent(rez,aux->parrent_id); //se cauta parintele lui aux, implicit nenul
		if(parrent!=NULL)
			parrent->v=addLast(parrent->v,aux);
		server=server->next;
	}
}

void printAddresses(Node *tree,FILE *fp) //afiseaza adresele rezolvabile de toate nodurile din tree
{
	Node aux=*tree;
	while(aux!=NULL)
	{
		fprintf(fp,"%d",aux->key);
		Address adrs=aux->adr;
		while(adrs!=NULL)
		{
			fprintf(fp," %s",adrs->name);
			adrs=adrs->next;
		}
		aux=aux->next;
		if(aux!=NULL)
			fprintf(fp,"\n");
	}
}

int findAddress(Node *element, char *adr) //cauta sirul adr in lista de adrese rezolvabile a nodului element
{
	Address adrs=(*element)->adr;
	while(adrs!=NULL)
	{
		if(strcmp(adrs->name, adr)==0)
			return 1;
		adrs=adrs->next;
	}
	return 0;
}

void listOfParents(Node *aux, Node *rez, KidsList *list) //variabila list va memora ca prim element parintele nodului aux din arbore
{														 
	if(*aux!=NULL)
	{
		Node parent=getParrent(rez, (*aux)->parrent_id); //parintele nodului aux din arbore
		if(parent!=NULL)
		{	
			*list=addLast(*list, parent); //la fiecare autoapel al functiei, se adauga in lista list, parintele ultimul element memorat in lista list
			listOfParents(&parent, rez, list);
		}
	}
}

void initHierarchy(Node *element,KidsList *lst,Node *tree) //adauga in listele de adrese rezolvabile ale nodurilor din lista lst adresele rezolvabile din lista de adrese a nodului element
{
	KidsList aux=*lst; //cu aux se parcurge fiecare nod din lista lst
	while(aux!=NULL)
	{
		Node parent=convertToNode(tree,aux->key); //parent este nodul din arbore cu valoarea aux->key
		Address adrs=(*element)->adr; //adrs este lista de adrese a nodului pentru care trebuie adaugate noi adrese
		if(parent!=*element) 
			while(adrs!=NULL)
			{
				if(findAddress(&parent,adrs->name)==0) //daca adresa pe care dorim sa o adaugam nu a fost deja adaugata in lista de adrese
					parent->adr=addAddress(parent->adr,adrs->name);
				adrs=adrs->next;
			}
		aux=aux->next;
		freeMem1(adrs);
		adrs=NULL;
	}
	freeMem3(aux);
	aux=NULL;
}

Node *readTree()
{
	static Node *rez=NULL; //lista care va memora nodurile arborelui
	FILE *f=fopen("tree.in","r");
	int n=0; //numarul de noduri din arbore
	fscanf(f,"%d",&n);
	Node tree; 
	int val=0,parrent_id=0,nr_kids=0;
	int i,j;
	rez=(Node *)calloc(n,sizeof(Node));
	for(i=0;i<n;i++) //se vor memora cele n noduri in arbore
	{
		tree=(node*)malloc(sizeof(node));
		tree->key=0;
		tree->parrent_id=0;
		tree->adr=NULL;
		fscanf(f,"%d%d%d",&val,&parrent_id,&nr_kids);
		for(j=0;j<nr_kids;j++) //citirea adreselor rezolvabile
		{	
			char *aux=(char *)malloc(10*sizeof(char));
			fscanf(f,"%s",aux);
			tree->adr=addAddress(tree->adr,aux);
			free(aux);
			aux=NULL;
		}
		tree->key=val;
		tree->parrent_id=parrent_id;
		createNode(tree->key,tree->parrent_id,&(tree->adr),rez); //adauga un nou nod in arbore
		free(tree);
	}
	addKids(rez); //adauga copiii fiecarui nod din lista
	fclose(f);
	return rez;
}

void printTree(Node *tree) //afiseaza tree.out fiecare nod din arbore si copiii acestuia
{
	FILE *fp=fopen("tree.out","w");
	Node aux;
	Node server=*tree;
	while(server!=NULL)
	{
		aux=server;
		KidsList aux1=aux->v; //lista auxiliara cu ajutoarul careia se parcurg copiii unui nod
		fprintf(fp,"%d",aux->key);
		if(aux1!=NULL)
			fprintf(fp," ");
		while(aux1!=NULL)
		{
			if(server->next!=NULL && aux1->next!=NULL) //nu este ultimul element din lista
				fprintf(fp,"%d ",aux1->key);
			else
			{
				if(aux1->next==NULL)
					fprintf(fp,"%d",aux1->key);
				else if(server->next==NULL && (aux1->next->next==NULL || aux1->next->next->next==NULL))
					fprintf(fp,"%d ",aux1->key);
			}
			aux1=aux1->next;
		}
		if(server->next!=NULL)
			fprintf(fp,"\n");
		server=server->next;
	}
	fclose(fp);
}

int number(int x) //returneaza numarul de cifre al lui x
{
	int k=1;
	while(x>0)
	{
		k=k+1;
		x=x/10;
	}
	return k;
}

Node findUserServer(Node *tree,int key) //returneaza nodul din arbore care contine in lista sa de useri un anume user cu val key
{
	Node aux=*tree;
	UsersList list=aux->users;
	int ok=0; //ok devine 1 atunci cand este gasit userul cautat
	while(aux!=NULL)
	{
		list=aux->users;
		while(list!=NULL && ok==0) //atata timp cat nu s-a ajuns la finalul listei de useri si nu a fost gasit userul cautat
		{
			if(list->key==key)
			{
				ok=1;
				break;
			}
			list=list->next;
		}
		if(ok==0)
			aux=aux->next;
		else
			break;
	}
	if(list==NULL)
		return NULL;
	return convertToNode(tree,aux->key);
}


void goToAddress(Node server,char *address,Node *tree,FILE *fp) //afiseza nodurile parcurse pana la gasirea adresei address, pornind din nodul server
{
	fprintf(fp,"%d",server->key); //id-ul nodului din care se incepe cautarea(nodul la care este legat userul)
	Address list1=server->adr;
	int ok=0;
	while(list1!=NULL) //se cauta adresa in lista de adrese rezolvabile de nodul in care se afla userul
	{
		if(strcmp(address,list1->name)==0) 
			ok=1;
		list1=list1->next;
	}
	freeMem1(list1);
	list1=NULL;
	if(ok==1); 
	else
	{
		KidsList list=NULL;
		listOfParents(&server,tree,&list); //list memoreaza o lista a nodurilor (din arborele tree) situate pe nivelurile superioare nodului server
		KidsList aux=list; 
		while(aux!=NULL)
		{
			Node element=convertToNode(tree,aux->key); //element este nodul din arbore cu id-ul aux->key
			Address adrs=element->adr;
			int ok=0;
			while(adrs!=NULL && ok==0)
			{
				if(strcmp(address,adrs->name)==0) // a fost gasita adresa cautata de user
				{
					ok=1;
					break;
				}	
				adrs=adrs->next;
			}
			if(ok==0) //atata timp cat adresa nu a fost inca gasita, se adauga adresa cautata in listele de adrese ale nodurilor prin care se trece 
			{
				fprintf(fp," %d",element->key); //se afiseaza nodurile parcurse pana la gasirea adresei
				element->adr=addAddress(element->adr,address); 												
			}
			else //la gasirea adresei aceasta se adauga in lista de adrese rezolvabile a nodului initial de la care s-a inceput cautarea
			{
				server->adr=addAddress(server->adr,adrs->name);
				fprintf(fp," %d",element->key); 
				break;
			}
			aux=aux->next;
		}
		freeMem3(list);
		list=NULL;
	}
}

int findUser(UsersList list, int key)  //returneaza 1 daca userul cu id_ul key se gaseste in lista de useri list
{
	UsersList aux=list;
	while(aux!=NULL)
	{
		if(aux->key==key)
			return 0;
		aux=aux->next;
	}
	return 1;
}

void deleteNodeFromTree(Node *tree,int key) //elimina din arbore nodul cu id_ul key
{
	Node aux=*tree;
    Node aux1=aux;
    while(aux->next!=NULL && aux->key!=key)
    {
     	aux1=aux;
        aux=aux->next;
    }
    aux1->next=aux->next;
    freeMem3(aux->v);
    freeMem2(aux->users);
    freeMem1(aux->adr);
    free(aux);
    aux=NULL;
}

void moveUsers(int key,Node *tree) //copiaza userii din nodul cu valoarea key in nodul parinte al acestuia
{
	Node server=convertToNode(tree,key);
	if(server!=NULL)
	{
		KidsList list=NULL;
		listOfParents(&server,tree,&list);
		if(list!=NULL)
		{
			Node element=convertToNode(tree,list->key);
			UsersList user=server->users;
			while(user!=NULL)
			{
				addUser(tree,element->key,user->key);
				user=user->next;
			}
		}
		freeMem3(list);
		list=NULL;
	}
}

void moveKids(int defectNode,Node *tree) //modifica valoare retinuta in parrent_id pentru toti copiii unui nod sters(nodul identificat cu id_ul defectNode)
{
	Node server=convertToNode(tree,defectNode); //server este nodul din arbore cu id-ul defectNode
	KidsList list_parr=NULL;
	listOfParents(&server,tree,&list_parr); //list_par este lista nodurilor din arbore aflate pe niveluri superioare nodului server
	int new_parr=0;
	KidsList aux=list_parr;
	while(aux!=NULL && new_parr==0) //new_par retine id_ul parintelui nodului server
	{
		new_parr=aux->key;
		aux=aux->next;
	}
	freeMem3(list_parr);
	if(server!=NULL) 
	{
		KidsList list=NULL;
		list=server->v;
		while(list!=NULL)
		{
			Node element=convertToNode(tree,list->key);
			if(element!=NULL)
				element->parrent_id=new_parr; //noul parrent_id al copiilor unui nod sters este id-ul parintelui nodului care a fost sters
			list=list->next;
		}
	}
}

void usersConnection(Node *tree,FILE *fp)
{
	FILE *fUser=fopen("users.in","r");
	FILE *fSearch=fopen("queries.in","r");
	int nr_user,user_id,user_server,nr_commands;
	int current_user;
	int defectNode;
	int i,j;
	char *address,command[50];
	fscanf(fUser,"%d",&nr_user);
	for(i=1;i<=nr_user;i++) //sunt memorati(in campul users din structura Node) utilizatorii legati la fiecare nod din arbore
	{
		fscanf(fUser,"%d",&user_id);
		fscanf(fUser,"%d",&user_server);
		if(findUserServer(tree,user_id)==NULL) //daca userul nu a fost deja adaugat in lista de useri a unui nod
			addUser(tree,user_server,user_id); //adauga userul identificat prin user_id la lista de useri a nodului user_server
	}
	fseek(fUser, 0, SEEK_SET); //seteaza indicatorul de pozitie la inceput fisierului
	fscanf(fSearch,"%d\n",&nr_commands); //nr_commands reprezinta numarul de cereri (q sau f) 
	for(j=1;j<=nr_commands;j++)
	{
		fgets(command,100,fSearch);
		current_user=atoi(command+2);
		address=command+number(current_user)+2;
		address[strlen(address)-1]='\0';
		if(address[0]==' ')
			address=address+1;
		if(command[0]=='q')
		{
			Node rez=findUserServer(tree,current_user); //rez reprezinta nodul din arbore la care este legat userul cu id-ul current_user
			goToAddress(rez,address,tree,fp);
			fprintf(fp,"\n");
		}
		if(command[0]=='f')
		{
			defectNode=atoi(command+2);
			moveUsers(defectNode,tree); //copiaza users din nodul care urmeaza a fi sters in lista de useri a parintelui acestuia
			moveKids(defectNode,tree); //copiaza copiii nodului sters in lista de copii a nodului sau parinte
			deleteNodeFromTree(tree,defectNode); //sterge nodul cu id-ul defectNode din arbore
		}
	}
}

int main()
{
	Node *t;
	t=readTree();
	printTree(t);
	FILE *f=fopen("hierarchy.out","w");
	Node server=NULL;
	server=*t;
	KidsList list=NULL;
	while(server!=NULL)
	{
		listOfParents(&(server),t,&list); //list este o lista a nodurilor din arbore situate pe niveluri superioare nodului server
		if(list!=NULL)
			initHierarchy(&(server),&list,t); //adauga in listele de adrese ale elementelor din lista list adresele din lista de adrese a nodului server 
		server=server->next;         
		freeMem3(list);
		list=NULL;
	}
	printAddresses(t,f);
	fclose(f);
	FILE *fOut = fopen("queries.out","w");
	usersConnection(t,fOut);
	fclose(fOut);
	freeMem(t);
	t = NULL;
	return 0;
}

