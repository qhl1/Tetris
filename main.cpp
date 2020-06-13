#include<SFML/Graphics.hpp>//图像处理
#include<SFML/Audio.hpp>//声音
#include<time.h>//处理时间的头文件
using namespace sf;
int blocks[7][4] = {
	1,3,5,7,//1
	2,4,5,7,//Z1
	3,5,4,6,//Z2
	3,5,4,7,//T
	2,3,5,7,//L
	3,5,7,6,//J
	2,3,4,5,//田
};


const int ROW_COUNT = 20;
const int COL_COUNT = 10;
//游戏区的表示
//table[i][j]==0 表示第i行第j列，是空白的，否则表示有方块
//table[i][j]==1 表示有方块，而且是第1种方块(使用第一种颜色)
int table[ROW_COUNT][COL_COUNT] = { 0 };


int blockIndex;//表示当前方块种类
//游戏方块表示
struct Point {
	int x,y;
}curBlock[4],bakBlock[4];

//表示下降速度
const float SPEED_NORMAL = 0.3;
const float SPEED_QUICK = 0.03;
float delay = SPEED_NORMAL;

//表示音效
Sound sou;

//显示分数
Font font;
Text textScore;
int score = 0;

//检查当前方块合法性
bool check()
{
	for (int i = 0; i < 4; i++)
	{

		if (curBlock[i].x < 0 || curBlock[i].x >= COL_COUNT || curBlock[i].y >= ROW_COUNT || table[curBlock[i].y][curBlock[i].x] != 0)
		{
			return false;
		}
	}
	return true;
}
void moveLeftRight(int offset)
{
	for (int i = 0; i < 4; i++)
	{
		bakBlock[i] = curBlock[i];
		curBlock[i].x += offset;
	}
	if (!check())
	{
		for (int i = 0; i < 4; i++)
		{
			curBlock[i] = bakBlock[i];
		}
	}
}
//旋转
void doRotate()
{
	if (blockIndex == 7)
	{
		return;
	}
	//先备份
	for (int i = 0; i < 4; i++)
	{
		bakBlock[i] = curBlock[i];
	}
	Point p = curBlock[1];//旋转中心
	/*
		x坐标计算：p.x-a[i].y+p.y
		y坐标计算：a[i].x-p.x+p.y
	*/
	for (int i = 0; i < 4; i++)
	{
		Point tmp = curBlock[i];
		curBlock[i].x = p.x - tmp.y + p.y;
		curBlock[i].y = tmp.x - p.x + p.y;
	}
	//检查合法性
	if (!check())
	{
		for (int i = 0; i < 4; i++)
		{
			curBlock[i] = bakBlock[i];
		}
	}
}
void keyEvent(RenderWindow *window) {
	bool rotate = false;//是否旋转
	int dx = 0;
	Event e;//事件变量
	//pollEvent从事件队列中取出一个事件
	//如果没有事件了，就返回false
	while (window->pollEvent(e))
	{
		if (e.type == Event::Closed)
		{
			window->close();
		}
		if (e.type == Event::KeyPressed)
		{
			switch (e.key.code)
			{
				case Keyboard::Up:rotate = true; break;
				case Keyboard::Left:dx = -1; break;
				case Keyboard::Right:dx = 1; break;
				default:break;
			}
		}
		//处理下降
		if (Keyboard::isKeyPressed(Keyboard::Down))
		{
			delay = SPEED_QUICK;
		}
		if (dx != 0) 
		{
			moveLeftRight(dx);
		}
		if (rotate)
		{
			doRotate();
		}
	}
}

void newBlock() {
	//获取一个随机值（1-7）
	blockIndex = 1 + rand() % 7;
	int n = blockIndex - 1;
	//序号%2就是x坐标
	//序号/2就是y坐标
	for (int i = 0; i < 4; i++)
	{
		curBlock[i].x = blocks[n][i] % 2;
		curBlock[i].y = blocks[n][i] / 2;
	}
}
void drawBlocks(Sprite *spriteBlock,RenderWindow *window) {
	//1.已经降落到底部的俄罗斯方块
	for (int i = 0; i < ROW_COUNT; i++)
	{
		for (int j = 0; j < COL_COUNT; j++)
		{
			if (table[i][j]!=0)
			{
				//画小方块
				//需要先使用Sprite表示完整方块图片
				spriteBlock->setTextureRect(IntRect(table[i][j] * 18, 0, 18, 18));
				spriteBlock->setPosition(j * 18, i * 18);
				//设置偏移量
				spriteBlock->move(28,31);
				window->draw(*spriteBlock);
			}
		}
	}


	//2.降落中的方块
	for (int i = 0; i < 4; i++)
	{
		spriteBlock->setTextureRect(IntRect(blockIndex * 18, 0, 18, 18));
		spriteBlock->setPosition(curBlock[i].x * 18, curBlock[i].y * 18);
		spriteBlock->move(28, 31);
		window->draw(*spriteBlock);
	}

}

void drop()
{
	for (int i = 0; i < 4; i++)
	{
		bakBlock[i] = curBlock[i];
		curBlock[i].y += 1;
	}
	if (check() == false)
	{
		//固化处理
		for (int i = 0; i < 4; i++)
		{
			table[bakBlock[i].y][bakBlock[i].x] = blockIndex;
		}
		//产生一个新方块
		newBlock();
	}
}
void cleanLine()
{
	int k = ROW_COUNT - 1;
	for (int i = ROW_COUNT - 1; i > 0; i--)
	{
		int count = 0;
		for (int j = 0; j < COL_COUNT; j++)
		{
			if (table[i][j])
			{
				count++;
			}
			table[k][j] = table[i][j];
		}
		if (count < COL_COUNT)
		{
			k--;
		}
		else
		{
			score += 10;
			sou.play();
		}
	}
	char tmp[16];
	sprintf_s(tmp, "%d", score);
	textScore.setString(tmp);
}
void initScore() {
	if (!font.loadFromFile("Sansation.ttf")) {
		exit(1);
	}

	textScore.setFont(font); // font is a sf::Font
	textScore.setCharacterSize(30);// set the character size
	textScore.setFillColor(sf::Color::White); // set the color
	textScore.setStyle(sf::Text::Bold); // set the text style
	textScore.setPosition(255, 175); //显示位置
	textScore.setString("0");
}
int main()
{
	srand(time(0));//生成一个随机种子
	//背景音乐
	Music music;
	if (!music.openFromFile("Elizabeth Naccarato - Unspoken.wav"))
	{
		return -1;
	}
	music.setLoop(true);
	music.play();

	SoundBuffer xiaochu;
	if (!xiaochu.loadFromFile("xiaochu.wav"))
	{
		return -1;
	}
	sou.setBuffer(xiaochu);



	//1.创建游戏窗口
	//1.1准备窗口的背景图片
	RenderWindow window(
		VideoMode(320, 416),//窗口模式，大小
		"Rock 2020");
	
	//2.添加游戏背景
	Texture t1;//把图片文件加载到内存
	t1.loadFromFile("image/bg2.jpg");
	Sprite spriteBg(t1);//根据图片创建精灵
	
	Texture t2;
	t2.loadFromFile("image/tiles.png");
	Sprite spriteBlock(t2);
	//相框
	Texture t3;
	t3.loadFromFile("image/frame.png");
	Sprite spriteFrame(t3);

	initScore();

	//渲染精灵

	window.draw(spriteBg);
	
	window.display();//刷新并显示窗口
	
	//生成第一个方块
	newBlock();
	
	//计时器
	Clock clock;

	float timer = 0;
	//进入游戏循环
	while (window.isOpen()) {
		//若窗口没有关闭
		//获取从clock被重启后到现在的时间
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;


		//等待用户按下按键
		keyEvent(&window);

		if (timer > delay)
		{
			//降落
			drop();//下降一位位置
			timer = 0;
		}

		cleanLine();

		delay = SPEED_NORMAL;
		//绘制游戏
		window.clear(Color::White);
		window.draw(spriteBg);
		window.draw(spriteFrame);
		//绘制方块
		drawBlocks(&spriteBlock,&window);
		window.draw(textScore); //显示分数
		window.display();//刷新并显示窗口
	}
	system("pause");
	return 0;
}