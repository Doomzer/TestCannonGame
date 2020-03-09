#include "stdafx.h"
#include "MainWidget.h"

MainWidget::MainWidget(const std::string & name, rapidxml::xml_node<>* elem): Widget(name)
{
	_timer = 0;
	_targets = nullptr;
	_cannon = nullptr;
	_targetsCount = 0;
	_timeLimit = 0;
	_speed = 0;
	_scene = START_SCENE;
	_bg = nullptr;
	_aim = nullptr;
	Init();
}

MainWidget::~MainWidget()
{
	if (_targets)
		_targets->Clear();
	delete _cannon;
}

void MainWidget::Init()
{
	// Считываем часть параметров из файла
	ReadFromFile();

	// Включаем стартовый экран
	_scene = START_SCENE;
	_cannon = new Cannon();
	_bg = Core::resourceManager.Get<Render::Texture>("background");
	_aim = Core::resourceManager.Get<Render::Texture>("Aim");
}

void MainWidget::ReadFromFile()
{
	IO::FileStream fs("input.txt");
	IO::TextReader reader(&fs);

	_targetsCount = ReadValue(&reader, "CountTarget");
	_speed = ReadValue(&reader, "Speed");
	_timeLimit = ReadValue(&reader, "Time");
}

int MainWidget::ReadValue(IO::TextReader *reader, char *arg)
{
	std::string str, name, value;

	str = reader->ReadAsciiLine();
	if (utils::ReadNvp(str, name, value) && name == arg)
	{
		return utils::lexical_cast<int>(value);
	}

	return 1;
}

void MainWidget::Draw()
{
	// Сначала рисуем фон
	Render::device.PushMatrix();
	Render::BeginColor(Color(255, 255, 255, 255));
	_bg->Draw();
	Render::EndColor();
	Render::device.PopMatrix();

	_effCont.Draw();

	std::string strTemp;

	// В зависимости от режима игры выводим на экран
	switch (_scene) 
	{
	case START_SCENE:
		break;
	case GAME_SCENE:
		_targets->Draw();
		_cannon->Draw();
		break;
	case TIME_OUT_SCENE:
		_targets->Draw();
		break;
	case WINNER_SCENE:
		_cannon->Draw();
		break;
	default:
		break;
	}

	// Выводим сообщения
	if (Render::isFontLoaded("ArialBig"))
	{
		Render::BindFont("ArialBig");
		switch (_scene)
		{
		case START_SCENE:		
			Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "Left click to start the game", 1.0f, CenterAlign);		
			break;
		case GAME_SCENE:
			if (_cannon->IsReloading())
				Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "Reloading, please wait", 1.0f, CenterAlign);
			else if(_cannon->NeedReload())
				Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "Need reload,\n right click to reload", 1.0f, CenterAlign);
			break;
		case TIME_OUT_SCENE:
			Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "Time's up!", 1.0f, CenterAlign);
			strTemp = "Targets " + utils::lexical_cast(_targetsCount - _targets->GetCountTargets()) + "/" + utils::lexical_cast(_targetsCount);
			Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50, strTemp, 1.0f, CenterAlign);
			Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 90, "Left click to restart the game", 1.0f, CenterAlign);
			break;
		case WINNER_SCENE:
			Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, "You have won,\n left click to restart the game", 1.0f, CenterAlign);
			break;
		default:
			break;
		}
	}

	Render::device.PushMatrix();
	// Запоминаем положение мышки
	IPoint mouse_pos = Core::mainInput.GetMousePos();

	Render::device.MatrixTranslate(math::Vector3(mouse_pos.x, mouse_pos.y, 0));
	IRect texRect = _aim->getBitmapRect();
	FRect rect(texRect), uv(0, 1, 0, 1);
	_aim->TranslateUV(rect, uv);
	Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.0f, -texRect.height / 2.0f, 0));

	_aim->Bind();
	Render::DrawQuad(rect, uv);
	Render::device.PopMatrix();

	// Вывод дополнительной информации
	if (Render::isFontLoaded("ArialMedium"))
	{
		Render::BindFont("ArialMedium");

		if (_scene == GAME_SCENE)
			Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 20, "Time left: " + utils::lexical_cast((int)(_timeLimit - _timer)), 1.0f, CenterAlign);
		else
			Render::PrintString(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 20, "Time left: 0", 1.0f, CenterAlign);

		if (_targets)
			Render::PrintString(10, 550, "Targets left: " + utils::lexical_cast(_targets->GetCountTargets()), 1.0f, LeftAlign);
		else
			Render::PrintString(10, 550, "Targets left: 0", 1.0f, LeftAlign);

		Render::PrintString(10, 580, "Ammo left: " + utils::lexical_cast(_cannon->AmmoLeft()), 1.0f, LeftAlign);
	}
	Render::device.PushMatrix();
	// Сдвигаем на текущую позицию
	Render::device.MatrixTranslate(math::Vector3(WINDOW_WIDTH / 2, 0, 0));
	Render::device.MatrixScale(0.2f);
	Render::device.PopMatrix();
}

void MainWidget::Update(float dt)
{
	// Обновим контейнер с эффектами
	_effCont.Update(dt);

	switch (_scene) 
	{
	case START_SCENE:
		break;
	case GAME_SCENE:
		// Проверяем столкновения ядер
		_targets->CheckBallHits();
		_targets->Update(dt);
		// Проверяем попадания по мишеням
		_cannon->CheckTargetsHit(_targets, _effCont);
		_cannon->Update(dt, _effCont);		
		// Есди время вышло, то переключаемся в режим 2
		if ((_timeLimit - _timer) <= 0) 
		{
			_scene = TIME_OUT_SCENE;
			_cannon->ClearBalls();
		}

		// Если мишеней не осталось , то переключаемся в режим 3
		if ((_targets->GetCountTargets() == 0)) 
		{
			_scene = WINNER_SCENE;
			_cannon->ClearBalls();
		}
		break;
	case TIME_OUT_SCENE:
		_targets->CheckBallHits();
		_targets->Update(dt);
		break;
	case WINNER_SCENE:
		_cannon->Update(dt, _effCont);
		break;
	default:
		break;
	}
	// Увеличиваем таймер на dt
	_timer += dt;
}

bool MainWidget::MouseDown(const IPoint & mouse_pos)
{
	if (Core::mainInput.GetMouseRightButton())
	{
		// При нажатии на правую кнопку мыши,
		if (_scene == GAME_SCENE)
			_cannon->ReloadBalls();
	}
	else
	{
		// При нажатии на левую кнопку мыши,
		switch (_scene) 
		{
		case START_SCENE:
			_targets = new Targets(_targetsCount, _timeLimit);
			// Переключаем режим в игровой
			_scene = GAME_SCENE;
			_timer = 0;
			break;

		case GAME_SCENE:
			// Отправляем оружию координаты клика
			_cannon->MouseDown(mouse_pos, _speed, _effCont);
			break;

		case TIME_OUT_SCENE:
		case WINNER_SCENE:
			_targets->Clear();
			_targets->Init();
			// Переключаемся в игровой режим
			_scene = GAME_SCENE;
			_timer = 0;
			break;
		default:
			break;
		}
	}
	return false;
}