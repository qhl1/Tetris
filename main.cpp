#include<SFML/Graphics.hpp>//ͼ����
#include<SFML/Audio.hpp>//����
#include<time.h>//����ʱ���ͷ�ļ�
using namespace sf;
int blocks[7][4] = {
	1,3,5,7,//1
	2,4,5,7,//Z1
	3,5,4,6,//Z2
	3,5,4,7,//T
	2,3,5,7,//L
	3,5,7,6,//J
	2,3,4,5,//��
};


const int ROW_COUNT = 20;
const int COL_COUNT = 10;
//��Ϸ���ı�ʾ
//table[i][j]==0 ��ʾ��i�е�j�У��ǿհ׵ģ������ʾ�з���
//table[i][j]==1 ��ʾ�з��飬�����ǵ�1�ַ���(ʹ�õ�һ����ɫ)
int table[ROW_COUNT][COL_COUNT] = { 0 };


int blockIndex;//��ʾ��ǰ��������
//��Ϸ�����ʾ
struct Point {
	int x,y;
}curBlock[4],bakBlock[4];

//��ʾ�½��ٶ�
const float SPEED_NORMAL = 0.3;
const float SPEED_QUICK = 0.03;
float delay = SPEED_NORMAL;

//��ʾ��Ч
Sound sou;

//��ʾ����
Font font;
Text textScore;
int score = 0;

//��鵱ǰ����Ϸ���
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
//��ת
void doRotate()
{
	if (blockIndex == 7)
	{
		return;
	}
	//�ȱ���
	for (int i = 0; i < 4; i++)
	{
		bakBlock[i] = curBlock[i];
	}
	Point p = curBlock[1];//��ת����
	/*
		x������㣺p.x-a[i].y+p.y
		y������㣺a[i].x-p.x+p.y
	*/
	for (int i = 0; i < 4; i++)
	{
		Point tmp = curBlock[i];
		curBlock[i].x = p.x - tmp.y + p.y;
		curBlock[i].y = tmp.x - p.x + p.y;
	}
	//���Ϸ���
	if (!check())
	{
		for (int i = 0; i < 4; i++)
		{
			curBlock[i] = bakBlock[i];
		}
	}
}
void keyEvent(RenderWindow *window) {
	bool rotate = false;//�Ƿ���ת
	int dx = 0;
	Event e;//�¼�����
	//pollEvent���¼�������ȡ��һ���¼�
	//���û���¼��ˣ��ͷ���false
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
		//�����½�
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
	//��ȡһ�����ֵ��1-7��
	blockIndex = 1 + rand() % 7;
	int n = blockIndex - 1;
	//���%2����x����
	//���/2����y����
	for (int i = 0; i < 4; i++)
	{
		curBlock[i].x = blocks[n][i] % 2;
		curBlock[i].y = blocks[n][i] / 2;
	}
}
void drawBlocks(Sprite *spriteBlock,RenderWindow *window) {
	//1.�Ѿ����䵽�ײ��Ķ���˹����
	for (int i = 0; i < ROW_COUNT; i++)
	{
		for (int j = 0; j < COL_COUNT; j++)
		{
			if (table[i][j]!=0)
			{
				//��С����
				//��Ҫ��ʹ��Sprite��ʾ��������ͼƬ
				spriteBlock->setTextureRect(IntRect(table[i][j] * 18, 0, 18, 18));
				spriteBlock->setPosition(j * 18, i * 18);
				//����ƫ����
				spriteBlock->move(28,31);
				window->draw(*spriteBlock);
			}
		}
	}


	//2.�����еķ���
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
		//�̻�����
		for (int i = 0; i < 4; i++)
		{
			table[bakBlock[i].y][bakBlock[i].x] = blockIndex;
		}
		//����һ���·���
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
	textScore.setPosition(255, 175); //��ʾλ��
	textScore.setString("0");
}
int main()
{
	srand(time(0));//����һ���������
	//��������
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



	//1.������Ϸ����
	//1.1׼�����ڵı���ͼƬ
	RenderWindow window(
		VideoMode(320, 416),//����ģʽ����С
		"Rock 2020");
	
	//2.�����Ϸ����
	Texture t1;//��ͼƬ�ļ����ص��ڴ�
	t1.loadFromFile("image/bg2.jpg");
	Sprite spriteBg(t1);//����ͼƬ��������
	
	Texture t2;
	t2.loadFromFile("image/tiles.png");
	Sprite spriteBlock(t2);
	//���
	Texture t3;
	t3.loadFromFile("image/frame.png");
	Sprite spriteFrame(t3);

	initScore();

	//��Ⱦ����

	window.draw(spriteBg);
	
	window.display();//ˢ�²���ʾ����
	
	//���ɵ�һ������
	newBlock();
	
	//��ʱ��
	Clock clock;

	float timer = 0;
	//������Ϸѭ��
	while (window.isOpen()) {
		//������û�йر�
		//��ȡ��clock�����������ڵ�ʱ��
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;


		//�ȴ��û����°���
		keyEvent(&window);

		if (timer > delay)
		{
			//����
			drop();//�½�һλλ��
			timer = 0;
		}

		cleanLine();

		delay = SPEED_NORMAL;
		//������Ϸ
		window.clear(Color::White);
		window.draw(spriteBg);
		window.draw(spriteFrame);
		//���Ʒ���
		drawBlocks(&spriteBlock,&window);
		window.draw(textScore); //��ʾ����
		window.display();//ˢ�²���ʾ����
	}
	system("pause");
	return 0;
}