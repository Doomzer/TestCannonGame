#pragma once
#include "Cannon.h"

enum SCENE
{
	START_SCENE,	  // �����
	GAME_SCENE,		  // ����
	TIME_OUT_SCENE,	  // ����� �����
	WINNER_SCENE	  // ������
};

class MainWidget : public GUI::Widget
{
public:
	MainWidget(const std::string& name, rapidxml::xml_node<>* elem);
	~MainWidget();

	void Draw() override;
	void Update(float dt) override;

	bool MouseDown(const IPoint& mouse_pos) override;

	static const int WINDOW_WIDTH = 1024;
	static const int WINDOW_HEIGHT = 768;

private:
	void Init();
	// ������ input.txt
	void ReadFromFile(); 
	int ReadValue(IO::TextReader *reader, char *arg);

	Targets	*_targets;
	Cannon *_cannon;

	int		_targetsCount;
	float	_timeLimit;
	float	_speed;
	float	_timer;
	SCENE	_scene;

	// ��������� � ���������
	EffectsContainer	_effCont;

	Render::Texture		*_bg;
	Render::Texture		*_aim;
};
