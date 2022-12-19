int a[10];
void main()
{
 int n,i,m;
 print("");
 getid(a);
 n=a[0];
 for ( i=0;i<10;i++)
     if (a[i]>n)
         n=a[i];
 print("\n");
 print(n);
}
//Miller A.
