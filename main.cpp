#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

#define speed 14.5
#define pexilsPerCm 28.5
#define scale 8
#define endl '\n'

using namespace cv;
using std::cin;
using std::cout;
using std::vector;
using std::pair;
using std::make_pair;
using std::string;
using std::queue;

Mat image; //Declaring the image globally to make it easier to use with functions.
struct cord
{
	//making a struct for coordinates
	short x;
	short y;
};

cord parent[3000][3000];
bool visit[3000][3000];

vector <pair <int, int> > v, inputToSecond; //to parse cordinates to 2nd proram
vector <pair <string, int> > path;

cord findFirst();

void draw(int pr, int pc, int firstr, int firstc);

/* 2nd program starts here */

enum{RIGHT,LEFT,UP,DOWN,INVALID}; //Directions Flags

string s[4] = {"R" , "L", "U", "D"}; //to print
int dir[1000000];

int dir_det(int y0, int x0, int y1, int x1);

void set_counts();

string toString(int num);

void serial_transmissions();
/* 2nd program ends here */

int main()
{
	//Entering the name/path of the file that contains the map.
	cout << "Enter the name/relative path of the file that contains the map : ";
	string mazeName;
	cin >> mazeName;
	image = imread(mazeName,1);

	//Entering the rf communication channel that the system is connected with the Arduino on.
	string channel;
	cout << "Enter the Arduino channel number: "; cin >> channel;
   string toFile = "\" >> /dev/rfcomm" + channel;
   cout << "The device is connected at " << toFile << "\"" << endl;

	bool done = false;
	int startRow, startCol, currentr, currentc, dr, dc;
	startCol = findFirst().x; //First coloumn of black pixels to avoid white borders
	startRow = findFirst().y; //First row of black pixels to avoid white borders
	cout << startCol << ' ' << startRow << '\n';
	for(int i=startCol+2; i<image.cols; i++)
	{
		if(image.at<Vec3b>(startRow,i)[0] >= 225 && image.at<Vec3b>(startRow,i)[1] && image.at<Vec3b>(startRow,i)[2] >= 225)//searching for the maze entrance
		{
			startCol=i+1;
			break;
		}
	}

	cout << startCol << ' ' << startRow << '\n';
	queue < pair<int,int> > q;
	q.push(make_pair(startRow,startCol));
	//BFS + intest Path
	while(!q.empty())
	{
		currentr=q.front().first;//taking info from queue
		currentc=q.front().second;//taking info from queue
		cout<<"current r = "<<currentr<<" current c = "<<currentc<<endl;
		if(currentr == image.rows-1)// if we are at the ending line line of the maze
		{
			dr=currentr;
			dc=currentc;
			done=true;
		}

		if(currentr-1 >= 0 && currentr-1 >= startRow && image.at<Vec3b>(currentr-1,currentc)[0] >= 225 &&
		 image.at<Vec3b>(currentr-1,currentc)[1] >= 225 && image.at<Vec3b>(currentr-1,currentc)[2] >= 225 &&
		  visit[currentr-1][currentc]==0)//checking if pixel is white before adding it to the queue
		{
			q.push(make_pair(currentr-1,currentc));
			parent[currentr-1][currentc].x=currentr;//saving parents of pixel x
			parent[currentr-1][currentc].y=currentc;//saving parents of pixel y
			visit[currentr-1][currentc]=1;//mark pixel as visited
		}

		if(currentr+1<image.rows&&image.at<Vec3b>(currentr+1,currentc)[0] >= 225 && image.at<Vec3b>(currentr+1,currentc)[1] >= 225 &&
		image.at<Vec3b>(currentr+1,currentc)[2] >= 225 && visit[currentr+1][currentc]==0)//checking if pixel is white before adding it to the queue
		{
			q.push(make_pair(currentr+1,currentc));
			parent[currentr+1][currentc].x=currentr;//saving parents of pixel x
			parent[currentr+1][currentc].y=currentc;//saving parents of pixel y
			visit[currentr+1][currentc]=1;//mark pixel as visited
		}

		if(currentc-1 >= 0 && image.at<Vec3b>(currentr,currentc-1)[0] >= 225 && image.at<Vec3b>(currentr,currentc-1)[1] >= 225 &&
		 image.at<Vec3b>(currentr,currentc-1)[2] >= 225 && visit[currentr][currentc-1]==0)//checking if pixel is white before adding it to the queue
		{
			q.push(make_pair(currentr,currentc-1));
			parent[currentr][currentc-1].x=currentr;//saving parents of pixel
			parent[currentr][currentc-1].y=currentc;//saving parents of pixel
			visit[currentr][currentc-1]=1;//mark pixel as visited
		}

		if(currentr<image.cols && image.at<Vec3b>(currentr,currentc+1)[0] >= 225 &&
		image.at<Vec3b>(currentr,currentc+1)[1] >= 225 && image.at<Vec3b>(currentr,currentc+1)[2] >= 225 &&
		visit[currentr][currentc+1]==0)//checking if pixel is white before adding it to the queue
		{
			q.push(make_pair(currentr,currentc+1));
			parent[currentr][currentc+1].x=currentr;//saving parents of pixel
			parent[currentr][currentc+1].y=currentc;//saving parents of pixel
			visit[currentr][currentc+1]=1;//mark pixel as visited
		}
		q.pop();
	}

	draw(dr, dc, startRow, startCol);//drawing the path found

	/*2nd program main statrs here*/
	int py,px,y,x;
	for(int i = 0; i < inputToSecond.size(); i++)
	{
		y = inputToSecond[i].first , x = inputToSecond[i].second;
		if(i)
			dir[i-1] = dir_det(py, px, y, x);
		py = y, px = x;
	}
	set_counts();
	serial_transmissions();
	/*2nd program main ends here*/

	namedWindow("image");//making window to show the solved maze
	imshow("image",image);
	waitKey(0);

    for(int i = 0; i < path.size(); i++)
	 {
        int val;
        string next = (string)(path[i].first) + " " + toString(path[i].second);
        cout << "The command in queue is:  " << next << endl;
        cout << "Would you like to send it? (Y/N): \nPress S to skip it" << endl;
        char yes; cin >> yes;
        val = path[i].second;

		  if (yes == 'S' || yes == 's')
		  	{
				continue;
			}
       else if(yes != 'y' && yes != 'Y')
		  {
			  i--;
            do
				{
                cout << "Direction and time: ";
                cin >> next;
            }while(next != "F" && next != "R" && next != "L" && next!= "B");

				cin >> val;
            next += (" "+toString(val));
        }

        string command = "echo \"" + next + toFile;
        system(("echo \"" + next + toFile + "\n").c_str());
        sleep((val+1000)/1000);
    }

	return 0;
}

cord findFirst()
{
	cord f;
	for(int i=0; i<image.rows; i++)
	{
		for(int j=0; j<image.cols; j++)
		{
			if(image.at<Vec3b>(i, j)[0] <= 25 && image.at<Vec3b>(i, j)[1] <= 25 && image.at<Vec3b>(i, j)[2] <= 25)//searching for the first column of black pixel and first row of black pixel
			{
				f.x=j;
				f.y=i;
				return f;
			}
		}
	}
}

void draw(int pr, int pc, int firstr, int firstc)
{
	int r, c;
	while(true)
	{
		inputToSecond.push_back(make_pair(pr, pc));
		image.at<Vec3b>(pr,pc)[0] = 0;
		image.at<Vec3b>(pr,pc)[1] = 255;
		image.at<Vec3b>(pr,pc)[2] = 0;
		image.at<Vec3b>(pr,pc+1)[0] = 0;
		image.at<Vec3b>(pr,pc+1)[1] = 255;
		image.at<Vec3b>(pr,pc+1)[2] = 0;
		image.at<Vec3b>(pr,pc+2)[0] = 0;
		image.at<Vec3b>(pr,pc+2)[1] = 255;
		image.at<Vec3b>(pr,pc+2)[2] = 0;

		if(pr == firstr && pc == firstc) break;
		r = pr;
		c = pc;
		pr = parent[r][c].x;
		pc = parent[r][c].y;
	}
}

int dir_det(int y0, int x0, int y1, int x1)
{

	/* 0 is previous, 1 is current
	 *
	 * Right
	 * y1 == y0 , x1 == x0 + 1
	 *
	 * Left
	 * y1 == y0 , x1 == x0 - 1
	 *
	 * Up
	 * y1 == y0 + 1 , x1 == x0
	 *
	 * Down
	 * y1 == y0 - 1 , x1 == x0
	 */

	if(y0 == y1){
		if(x1 == x0 + 1)
			return RIGHT;
		else return LEFT;
	}
	if(x0 == x1){
		if(y1 == y0 + 1)
			return DOWN;
		else return UP;
	}
	return INVALID; //will not pass here
}

void set_counts()
{
	int cur = dir[0], cnt = 1;
	for(int i = 1; i < inputToSecond.size() - 1; i++)
	{
		if(dir[i] != cur)
		{
			v.push_back(make_pair(cur, cnt));
			cnt = 0;
		}
		cnt++;
		cur = dir[i];
	}
}

string toString(int num)
{
    string ret = "";
    while(num){
        ret += (char)('0'+num%10);
        num /= 10;
    }
    reverse(ret.begin(), ret.end());
    return ret;
}

void serial_transmissions()
{
   path.push_back(make_pair("F", v[0].second / (pexilsPerCm * speed) * 1000 * scale));
	for(int i = 1; i < v.size(); i++)
	{
		string corner = s[ v[i-1].first ] + s[ v[i].first ];
		if(corner == "RU" || corner == "LD" || corner == "DR" || corner == "UL")
            path.push_back(make_pair("L", 620));

      if(corner == "RD" || corner == "LU" || corner == "DL" || corner == "UR")
            path.push_back(make_pair("R", 620));

        path.push_back(make_pair("F", v[i].second / (pexilsPerCm * speed) * 1000 * scale));
	}
}
