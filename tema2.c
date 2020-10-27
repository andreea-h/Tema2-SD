#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct address //structura de tip lista pentru memorarea adreselor rezolvabile de fiecare nod din arbore
{
	char *name;
	struct address *next;
}address;
typedef address *Address;

typedef struct usersList  //structura de tip lista pentru memorarea userilor 
{
	int key; //id-ul userului
	int server; //id-ul nodului la care este conectat un anumit user
	struct usersList *next;
}usersList;
typedef usersList *UsersList;

typedef struct node
{
	int key;
	Address adr; //lista care retine adresele rezolvabile de nod
	int nr_adrs; //numarul de adrese rezolvabile de catre nodul din arbore
	struct node *child; //pointer catre primul copil al unui nod
	struct node *sibling; //pointer catre fratele de pe acelasi nivel al copilului unui nod
}node;
typedef struct node *Node;

void freeAddressList(Node nod) //elibereaza memoria alocata pentru lista de adrese a unui nod
{ 
   Address aux=nod->adr; 
   Address aux1; 
   while (aux!=NULL)  
   { 
       aux1=aux->next; 
       free(aux->name); 
       free(aux);
       aux=aux1; 
   } 
   nod->adr=NULL;
} 

void freeAllAddresses(Node tree) //elibereaza memoria alocata pentru listele de adrese ale tuturor nodurilor din arbore
{
	if(tree==NULL)
		return;
	while(tree!=NULL)
	{
		freeAddressList(tree);
		if(tree->child!=NULL)
			freeAllAddresses(tree->child);
		tree=tree->sibling;
	}
}

void freeUserList(UsersList lst)  //elibereaza memoria alocata pentru lista de useri
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

void freeTree(Node root) //elibereaza memoria alocata pentru nodurile arborelui
{
	if(root!=NULL)
	{
		freeTree(root->child);
		freeTree(root->sibling);
		free(root);
	}
}

Node newNode(int x) //creeaza un nod nou in arbore, cu campul key egal cu x
{
	Node nou=(node*)malloc(sizeof(node));
	nou->key=x;
	nou->child=nou->sibling=NULL; //initial, nodul nu este legat de celelalte noduri din arbore
	nou->adr=NULL;
	return nou;
}

Node addSibling(Node nod, int data) //adauga fratele cu continut data la nodul nod, primit ca parametru
{ 
    if(nod==NULL) 
        return NULL;  
    Node aux=nod;
    while(aux->sibling!=NULL) 
        aux=aux->sibling;
    aux->sibling=newNode(data);
    return aux->sibling;
} 

Node addChild(Node nod, int data)
{ 
    if(nod==NULL) 
        return NULL; 
    if(nod->child!=NULL) //daca nodul la care se doreste adaugarea unui copil are deja adaugat cel putin un copil 
       	return addSibling(nod->child, data); //nodul se adauga ca frate al copilului deja inserat
    else
	{
		nod->child=newNode(data); //se creaza un nou nod copil
		return nod->child; 
	}
} 

int findAddress(Address nod, char *adr) //cauta sirul adr in lista de adrese rezolvabile a nodului element
{
	if(nod!=NULL)
	{
		while(nod!=NULL)
		{
			if(strcmp(nod->name,adr)==0)
				return 1;    //functia returneaza 1 daca adresa a fost gasita, 0 in caz contrar
			nod=nod->next;
		}
	}
	return 0;
}

Address addAddress(Address lst,char *adr) //adauga un nou element in lista de adrese
{
	Address nou=(address *)calloc(1,sizeof(address));
	nou->name=(char *)malloc(strlen(adr)+1);
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

void initAddressList(Address *lst,char **adr,int nr_adr) //initializeaza lista de adrese a unui nod cu nr_kids adrese rezolvabile,cele preluate din fisierul tree.in
{
	int i;
	for(i=0;i<nr_adr;i++)
	{
		if(findAddress(*lst,adr[i])==0) //daca adresa nu am mai fost adaugat in lista de adrese a unui nod (memorata in lista lst)
			*lst=addAddress(*lst,adr[i]);
	}
}

void printHierarchy(Node tree,FILE *fp) //afiseaza adresele rezolvabile de fiecare nod din arbore, in urma initializarii ierarhiei
{
	if(tree==NULL)
		return;
	while(tree!=NULL)
	{
		fprintf(fp,"%d",tree->key);
		Address list=tree->adr; //se parcurge lista de adrese rezolvabile a nodului curent, afisand adresele
		while(list!=NULL)
		{	
			fprintf(fp," %s",list->name);
			list=list->next;
		}
		fprintf(fp,"\n");
		if(tree->child!=NULL)
			printHierarchy(tree->child,fp);
		tree=tree->sibling;
	}
}

void initTree(Node root,int v[],int n,int index,Node poz,char ***adr,int f[]) //initializeaza structura de arbore cu datele preluate din 'tree.in'
{
	int j=0;
	Node poz1;
	while(j<n)
	{
		if(v[j]==index)
		{
			poz1=addChild(poz,j); //adauga un copil cu valoarea key egala cu j nodului poz din arbore
			initAddressList(&(poz1->adr),adr[j],f[j]);
			poz1->nr_adrs=f[j];
			if(poz1!=NULL)
				initTree(root,v,n,poz1->key,poz1,adr,f);
		}
		j++;
	}
}

void putAddress(Node nod1,Node nod2) //adauga in lista de adrese a nodului nod1 adresele din lista de adrese a nodului nod 2
{
	Address aux=nod2->adr; //aux este lista de adrese a nodului nod2
	while(aux!=NULL)
	{
		if(aux->name!=NULL)
		if(findAddress(nod1->adr,aux->name)==0)
			nod1->adr=addAddress(nod1->adr,aux->name);
		aux=aux->next;
	}
}

void initHierarchy(Node tree) //initializarea ierarhiei de adrese
{
	Node aux;
	if(tree==NULL)
		return;
	initHierarchy(tree->child);
	if(tree->child!=NULL)
	{
		if(tree!=NULL && tree->child!=NULL)
			putAddress(tree,tree->child); //adauga adresele din lista copilului in lista de adrese a parintelui sau
		aux=tree->child->sibling;
		while(aux!=NULL)
		{
			if(tree!=NULL && aux!=NULL)
				putAddress(tree,aux);
			aux=aux->sibling;
		}
	}
	initHierarchy(tree->sibling);
}

void printKids(Node tree,FILE *fp) //afiseaza nodurile din arbore, urmate de copiii acestora
{
	Node aux;
	if(tree==NULL)
		return;
	fprintf(fp,"%d",tree->key);
	if(tree->child!=NULL)
	{
		fprintf(fp," %d",tree->child->key);
		aux=tree->child->sibling;
		while(aux!=NULL)
		{
			fprintf(fp," %d",aux->key);
			aux=aux->sibling;
		}
	}
	fprintf(fp,"\n");
	printKids(tree->child,fp);
	printKids(tree->sibling,fp);
}

void addUser(UsersList *list,int id_user,int id_server) //adauga un nou element in lista de useri list
{
	UsersList nou=(usersList *)malloc(sizeof(usersList)); 
	nou->key=id_user;
	nou->server=id_server;
	if(*list==NULL)
	{
		nou->next=NULL;
		*list=nou;
	}
	else
	{
		UsersList aux=*list;
		while(aux->next!=NULL)
			aux=aux->next;
		nou->next=NULL;
		aux->next=nou;
	}
}

int findUserServer(UsersList list,int key) //returneaza valoarea nodului la care este legat userul cu valoarea key
{
	UsersList aux=list;
	while(aux!=NULL && aux->key!=key)
		aux=aux->next;
	if(aux!=NULL)
		return aux->server;
	return 0;
}

void convertToNode(int key,Node tree,Node *server) //returneaza nodul din arbore cu valoarea key
{
	if(tree!=NULL)
	{
		if(tree->key==key)
		{
			*server=tree;
			return;
		}
		else
			if(tree->child!=NULL && tree->child->key==key)
			{
				*server=tree->child;
				return;
			}
		else if(tree->child!=NULL && tree->child->sibling!=NULL && tree->child->sibling->key==key)
		{
			*server=tree->child->sibling;
			return;
		}
		convertToNode(key,tree->child,server);
		convertToNode(key,tree->sibling,server);
	}
}

void getParrent(Node tree,Node *parrent,int server) //memoreaza in parrent adresa parintelui nodului cu valoarea server
{
	Node aux;
	if(tree==NULL)
		return;
	if(tree->child!=NULL)
	{
		if(tree->child->key==server && tree->child!=tree)
		{
			*parrent=tree;
			return;
		}
		aux=tree->child->sibling;
		while(aux!=NULL)
		{
			if(aux->key==server && aux!=tree)
			{
				*parrent=tree;
				return;
			}
			aux=aux->sibling;
		}
	}
	getParrent(tree->child,parrent,server);
	getParrent(tree->sibling,parrent,server);
}

void cachingAddress(Node tree,int init,char *address) //adauga adresa address in lista de adrese a nodului cu valoarea init din arbore
{
	Node start=NULL;
	convertToNode(init,tree,&start);
	start->adr=addAddress(start->adr,address);
}

void goToParentAddress(Node tree,Node *parrent,int user_server,char *address,FILE *fp,int init) //init este nodul initial de la care s-a inceput cautarea
{
	if(tree!=NULL)
	{
	Address list=(*parrent)->adr;
	int k=(*parrent)->key; 
	int ok=0;
	while(list!=NULL) //initial se cauta adresa in lista de adrese rezolvabile de nodul in care se afla userul
	{	
		if(strcmp(address,list->name)==0) 
			ok=1;
		list=list->next;
	}
		if(ok==1 || (*parrent)->key==0) //s-a gasit adresa cautata 
		{
			cachingAddress(tree,init,address); //se adauga adresa cautata in lista de adrese a nodului de la care s-a inceput cautarea
			fprintf(fp," %d",(*parrent)->key);
			return;
		}
		else
		{
			fprintf(fp," %d",k);
			Node server=NULL;
			convertToNode(k,tree,&server);
			server->adr=addAddress(server->adr,address);
			getParrent(tree,parrent,k); 
			goToParentAddress(tree,parrent,k,address,fp,init);
		}
	}
}

void goToAddress(int nod,char *address,Node tree,FILE *fp) //afiseaza in queries.out nodurile din arbore prin care se trece pana la gasirea adresei 'address', pornind din nodul nod
{
	fprintf(fp,"%d",nod);
	int ok=0;
	Node server=NULL;
	convertToNode(nod,tree,&server);
	Address list=server->adr;
	while(list!=NULL) //initial se cauta adresa in lista de adrese rezolvabile de nodul in care se afla userul
	{
		if(strcmp(address,list->name)==0) 
			ok=1;
		list=list->next;
	}
	if(ok==1);
	else //adresa se va cauta in nodurile din arbore situate pe nivelurile superioare si care sunt 'tati' pentru nodul curent
	{
		Node parrent=NULL;
		getParrent(tree,&parrent,nod); 
		goToParentAddress(tree,&parrent,nod,address,fp,nod);
	}
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

void moveUsers(int defectNode, UsersList list, Node tree)
{
	Node parrent=NULL; //parintele nodului defect
	getParrent(tree,&parrent,defectNode); 
	UsersList aux=list;
	while(aux!=NULL)
	{
		if(aux->server==defectNode)
			aux->server=parrent->key; //actualizeaza serverul defect cu serverul parinte al acestuia
		aux=aux->next;
	}
}

void moveKids(int defectNode,Node tree)
{
	Node parrent=NULL; //parintele nodului defect
	getParrent(tree,&parrent,defectNode); 
	Node server=NULL;
	convertToNode(defectNode,tree,&server); //server este nodul din tree cu id-ul defectNode
	if(server->child!=NULL)
	{
		addChild(parrent,tree->child->key);
		Node aux=server->child->sibling;
		while(aux!=NULL)
		{
			addChild(parrent,aux->key);
			aux=aux->sibling;
		}
	}
}

void deleteNode(Node server,Node tree) //sterge nodul server din arbore
{
	if(tree->child==NULL)  // nodul nu are copii, deci se va inlocui cu primul sau frate
	{
		Node parrent=NULL;
		getParrent(tree,&parrent,server->key);  //parrent este parintele nodului care trebuie sters
		parrent->child=parrent->child->sibling;
		free(server);
		server=NULL;
	}
}

void usersConnection(Node tree)
{
	UsersList users=NULL;
	FILE *fUser=fopen("users.in","r");
	FILE *fSearch=fopen("queries.in","r");
	FILE *fOut=fopen("queries.out","w");
	int nr_user,user_id,user_server,nr_commands;
	int current_user;
	int i,j;
	char *address,*command;
	fscanf(fUser,"%d",&nr_user);
	for(i=1;i<=nr_user;i++) //sunt memorati utilizatorii legati la fiecare nod din arbore
	{
		fscanf(fUser,"%d",&user_id);
		fscanf(fUser,"%d",&user_server);
		addUser(&users,user_id,user_server);//adauga un user cu id-ul user_id si nodul user_server in lista de useri 'users'
	}
	fseek(fUser, 0, SEEK_SET); //seteaza indicatorul de pozitie la inceput fisierului
	fscanf(fSearch,"%d\n",&nr_commands); //nr_commands reprezinta numarul de cereri (q sau f) 
	for(j=1;j<=nr_commands;j++)
	{
		command=(char *)malloc(100);
		fgets(command,100,fSearch);
		current_user=atoi(command+2);
		address=command+number(current_user)+2;
		address[strlen(address)-1]='\0';
		if(address[0]==' ')
			address=address+1;
		if(command[0]=='q')
		{
			int rez=findUserServer(users,current_user); //rez reprezinta valoarea nodului din arbore la care este legat userul cu id-ul current_user
			goToAddress(rez,address,tree,fOut);
			fprintf(fOut,"\n");
		}
		if(command[0]=='f')
		{
			int defectNode=atoi(command+2);
			moveUsers(defectNode,users,tree); //inlocuieste id_ul serverului userilor aflati in defectNode cu parintele lui defectNode
			moveKids(defectNode,tree); //copiaza copiii nodului sters in lista de copii a nodului sau parinte
			Node deleted=NULL;
			convertToNode(defectNode,tree,&deleted);
			deleteNode(deleted,tree); //sterge nodul cu valoarea defectNode din arbore (adica nodul memorat in deleted)
		}
		free(command);
	}
	freeUserList(users);
}

int main()
{
	int n=0; //numarul de noduri din arbore
	FILE *f=fopen("tree.in","r");
	fscanf(f,"%d",&n); //n resprezinta numarul de noduri din arbore
	int *v=(int *)malloc(n*sizeof(int)); //vectorul v memoreaza id-urile parintilor nodurilor din arbore in etapa intermediara a citirii
	int *fr=(int *)malloc(n*sizeof(int)); //vectorul fr memoreaza numarul de adrese rezolvabile de nodurile din arbore
	char ***address; //tablou tridimensional pentru memorarea adreselor rezolvabile de noduri
	int val=0,parrent_id=0,nr_adr=0; //val reprezinta id_ul nodului din arbore
	int i,j;
	address=(char ***)calloc(n,sizeof(char **));
	for(i=0;i<n;i++) 
	{
		fscanf(f,"%d%d%d",&val,&parrent_id,&nr_adr);
		fr[i]=nr_adr; //nr_adr reprezinta numarul de adrese rezolvabile de un nod
		address[i]=(char **)calloc(nr_adr,sizeof(char *));
		for(j=0;j<nr_adr;j++) //citirea adreselor rezolvabile de catre un nod
		{	
			char *aux=(char *)calloc(10,sizeof(char));
			fscanf(f,"%s",aux);
			address[i][j]=(char *)calloc(strlen(aux)+1,sizeof(char));
			strcpy(address[i][j],aux);
			free(aux);
			aux=NULL;
		}
		v[i]=parrent_id;
	}
	Node root=NULL;
	root=newNode(-1);
	Node main_root=root;
	root=addChild(root,0); //radacina arborelui va fi nodul cu valoarea 0
	Node aux=root;
	initTree(root,v,n,0,aux,address,fr); //contruieste arborele cu datele preluate din 'tree.in'
	initAddressList(&(root->adr),address[0],fr[0]);
	for(i=0;i<n;i++)
	{
		for(j=0;j<fr[i];j++) 
			free(address[i][j]);
		free(address[i]);
	}
	free(address);
	free(v);
	free(fr);

	FILE *fp=fopen("tree.out","w");
	FILE *fOut=fopen("hierarchy.out","w");
	printKids(aux,fp); //cerinta 1
	fclose(fp);
	initHierarchy(aux);
	printHierarchy(aux,fOut); //cerinta 2
	fclose(fOut);
	usersConnection(aux); //cerintele 3 si 4
	freeAllAddresses(aux);
	freeTree(root);
	free(main_root);
	return 0;
}



