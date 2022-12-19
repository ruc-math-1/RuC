void main()
{
 float m,n;
 print("");
 getid(n);
 m=n;
 while (abs(n*n-m)>1e-6)
     n=(n+m/n)/2;
 print("\n");
 print(n);
}
//Miller A.
