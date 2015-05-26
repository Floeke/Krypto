#include <iostream>
#define PRIME 127

class UInt {
public:
	unsigned int zahl;

#pragma region Konstruktoren
	//Default. Zahl = 0
	UInt() 
	{
		zahl = 0;
	}

	UInt(int x)
	{
		zahl = x;
	}	
#pragma endregion

#pragma region andere Funktionen (expm, isSquare, root)
	//Berechnet a^b
	UInt expm(UInt a, UInt b)
	{
		unsigned int m = b.zahl + PRIME - 1;
		UInt r = 1, a_ = a.zahl, b_ = b.zahl;
		while (m)
		{
			if (m & 1)
				r = r * a;
			a = a * a;
			m = m >> 1;
		}

		return r;
	}
	
	//Falls es ein Quadrat ist: Wurzel. Wenn nicht: 0.
	UInt root()
	{
		if (isSquare())
			return(expm(*this, (PRIME + 1) >> 2));
		else return UInt(0);
	}

	//Gibt true zurück wenn es ein Quadrat ist, ansonsten false
	bool isSquare()
	{
		return(expm(*this, (PRIME - 1) >> 1).zahl == 1);
	}
#pragma endregion

#pragma region Operatoren

	//(a+b)%PRIME. Funktioniert nur mit Mersennenprimzahlen
	UInt operator+(UInt x) 
	{
		unsigned int a = zahl, b = x.zahl;

		if (a < (b^PRIME))
			return UInt(a + b);
		if ((b^PRIME) <= a)
			return UInt(a - (b^PRIME));
		if ((a^PRIME) <= b)
			return UInt(b - (a^PRIME));
	}

	//(a-b)%PRIME. Funktioniert nur mit Mersennenprimzahlen
	UInt operator-(UInt x)
	{
		if (x.zahl == PRIME)
			return *this;
		return (UInt(x.zahl^PRIME) + *this);
	}

	//Bitshift nach links
	UInt operator<<(int x)
	{
		int y = this->zahl;
		return(UInt(y << x));
	}

	//Bitshift nach rechts
	UInt operator>>(int x)
	{
		int y = this->zahl;
		return (UInt(y >> x));
	}

	//(a*b)%PRIME. Funktioniert nur mit Mersennenprimzahlen
	UInt operator*(UInt x)
	{
		UInt temp = x.zahl;
		UInt r = 0;
		UInt that = this->zahl;
		while (temp.zahl)
		{
			if (temp.zahl & 1)
				r = r + that;
			that = that + that;
			temp = temp >> 1;
		}

		return r;
	}

	//(a/b)%PRIME. Funktioniert nur mit Mersennenprimzahlen
	UInt operator/(UInt x)
	{
		if (x.zahl == 0)
			return UInt(0);
		return *this * UInt(expm(x, UInt(PRIME - 2)));
	}

	//Unnötig zu erklären...
	bool operator==(UInt x)
	{
		if (zahl == x.zahl)
			return true;
		return false;
	}

	//Unnötig zu erklären...
	bool operator==(int x)
	{
		if (zahl == x)
			return true;
		return false;
	}

	//Überladener Modulo-Operator
	UInt operator%(UInt x)
	{
		return(UInt(zahl % x.zahl));
	}

	//Überladener Modulo-Operator
	UInt operator%(int x)
	{
		return(UInt(zahl%x));
	}

	//Berechnet das inverse Element
	UInt inverse()
	{
		return(UInt(zahl^PRIME));
	}

	//Einfache Zuweisung. Spart Konstruktor-Aufruf
	void operator=(int x)
	{
		zahl = x;
	}
#pragma endregion

};

class UPoint {

private:

	UInt a = 4; 
	UInt b = 20; //20

	//Berechnet den Y-Wert. 
	void generate_Point()
	{
		UInt y_square = x * x * x + a * x + b;
		if (y_square.zahl < 1)
			y = PRIME;
		else
			y = y_square.root();

		if (y_square.root() == -1)
			y = 0;
	}

	//Überprüft ob die Kurve legitim ist. 
	bool curve_is_legit(int _a, int _b)
	{
		UInt a_neu = _a;
		UInt b_neu = _b;
		if ((((UInt(4)*a_neu*a_neu*a_neu) + (UInt(27)*b_neu*b_neu)) % PRIME) == 0)
		{
			perror("Non-legit parameters!\n");
			return false;
		}
		return true;
	}

public:
	UInt x, y;

	//Ändert die Domainparameter (nur vom aktuellem Punkt!!), wenn diese legitim sind
	void change_domain_params(int _a, int _b)
	{
		if (!curve_is_legit(_a, _b))
			return;
		a = _a;
		b = _b;

	}

	void print_point()
	{
		if (y.zahl != 0)
		{
			printf("\n(%3d, %3d) & (%3d, %3d)", x, y, x, y.inverse());
		
		}
		else
		{
			printf("\nNot a point!");
		}
	}

#pragma region Konstruktoren

	//Default. x=0, y=0
	UPoint() 
	{
		x = 0;
		y = 0;
	}

	//Übergabe der X-Koordinate, berechnet Y automatisch
	UPoint(UInt x)
	{
		this->x = x.zahl;
		generate_Point();
	}
	
	//Übergabe der X-Koordinate, berechnet Y automatisch
	UPoint(int x)
	{
		this->x = x;
		generate_Point();
	}

	//Erstellt manuell einen Punkt. (Böse)
	UPoint(UInt x, UInt y)
	{
		this->x = UInt(x.zahl);
		this->y = UInt(y.zahl);
	}

	//Erstellt manuell einen Punkt. (Böse)
	UPoint(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
#pragma endregion

#pragma region Operatoren
	//A+B = C. Ja, wirklich. Eine elliptische Kurve.
	UPoint operator+(UPoint otherPoint)
	{
		if (otherPoint.x.zahl == 0 && otherPoint.y.zahl == 0)
			return *this;
		if (this->x.zahl == 0 && this->y.zahl == 0)
			return otherPoint;
		if ((this->y + otherPoint.y).zahl == 0)
			return UPoint(PRIME, PRIME);

		if (this->x.zahl == otherPoint.x.zahl)
		{
			if (this->y.zahl == otherPoint.y.zahl && this->y.zahl != 0 && otherPoint.y.zahl != 0)
			{
				UInt m = ((UInt(3) * this->x * this->x) + a) / (UInt(2) * this->y);
				UInt m2 = m*m;
				UInt x3 = m2 - (UInt(2) * this->x);
				UInt y3 = (m*(this->x - x3)) - this->y;
				return UPoint(x3, y3);
			}
			else
				return UPoint(PRIME, PRIME);
		}

		UInt m = (otherPoint.y - this->y)/(otherPoint.x - this->x);
		UInt _x = (m*m) - this->x - otherPoint.x;
		UInt _y = m*(this->x - _x ) - this->y;		

		return UPoint(_x, _y);
	}

	

#pragma endregion
};
 
#pragma region Least Common Divisor
int ggT(int zahl1, int zahl2)
{
	if (zahl2 == 0)
	{
		return zahl1;
	}

	return ggT(zahl2, zahl1%zahl2);
}

unsigned long long kageVau(unsigned long long zahl1, unsigned long long zahl2)
{
	return (zahl1*zahl2) / ggT(zahl1, zahl2);
}

unsigned long long kgV_array(unsigned long long zahlen[])
{
	unsigned long long kgV = 1;

	for (int i = 0; i < PRIME; i++)
	{
		if (zahlen[i]!=0)
			kgV = kageVau(kgV, zahlen[i]);
	}

	return kgV;
}
#pragma endregion


void main()
{
	UInt x, y;
	UPoint punkt1, punkt2;
	unsigned long long counter[127] = { 0 };
	int single_count = 0;
	//punkt1 = UPoint(37);
	
	punkt1 = UPoint(1);
	punkt2 = UPoint(punkt1.x, punkt1.y);

	for (int i = 0; i < 127; i++)
	{
		punkt1 = UPoint(i);
		if (punkt1.y.zahl != 0)
			single_count += 2;
	}

	printf("Your curve has %d points\n", single_count);

	for (int i = 0; i < 127; i++)
	{
		punkt1 = UPoint(i);
		punkt2 = UPoint(i);
		punkt1.print_point();
		single_count = 0;
		punkt1 = punkt1 + punkt2;
		while (true)
		{
			single_count++;
			punkt1 = punkt1 + punkt2;
			if ((punkt1.y.zahl == punkt2.y.zahl && punkt1.x.zahl == punkt2.x.zahl) || punkt1.y.zahl== PRIME)
			{
				//printf("So many counts: %d\n", single_count);
				counter[i] = single_count;
				break;
			}
		}
	}

	//while (true)
	//{
	//	printf("%d %d\n", punkt1.x.zahl, punkt1.y.zahl);
	//	punkt1 = punkt1 + punkt2;
	//	
	//	if (punkt1.x.zahl == punkt2.x.zahl || punkt1.x.zahl == PRIME || punkt1.y.zahl == PRIME || punkt1.y.zahl == (127 -punkt2.y.zahl))
	//		break;

	//	single_count++;
	//}

	//printf("single: %d\n", single_count);

	//



	//for (int i = 0; i < 127; i++)
	//{
	//	punkt1 = UPoint(i);
	//	punkt2 = UPoint(i);
	//	do
	//	{
	//		if (punkt1.y.zahl == 0)
	//			break;
	//		punkt1 = punkt2 + punkt1;
	//		if (punkt1.x.zahl == punkt2.x.zahl || punkt1.x.zahl == PRIME || punkt1.y.zahl == PRIME || punkt1.y.zahl == (127 - punkt2.y.zahl))
	//			break;
	//		//printf("(%d, %d)\n", punkt1.x.zahl, punkt1.y.zahl);			
	//		counter[i]++;
	//	} while (punkt1.x.zahl != PRIME && punkt1.y.zahl != PRIME && punkt1.x.zahl != punkt2.x.zahl && punkt1.x.zahl != (127-punkt2.x.zahl));
	//}

	//

	for (int i = 0; i < 127; i++)
		printf("\nCounter #%3d: %3d", i+1, counter[i]);

	printf("\n%llu ", kgV_array(counter));
	system("pause"); 
}