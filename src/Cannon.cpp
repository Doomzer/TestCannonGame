#include "stdafx.h"
#include "Cannon.h"
#include "MainWidget.h"

// ----------------------------- Cannonball Class ----------------------

Cannonball::Cannonball(float scale, float speed, float angle, EffectsContainer &effCont) 
{
	_effectLine = effCont.AddEffect("Line");	
	_scale = scale;
	_speed = speed;
	_angle = angle;
	_reboundCount = 0;
	_didReboundMin = false;
	_didReboundMax = false;
	Init();
}

void Cannonball::Init()
{
	_texture = Core::resourceManager.Get<Render::Texture>("Cannonball");
	// Установка начальной позиции
	_position = IPoint(MainWidget::WINDOW_WIDTH / 2, 75);

	// Добавление эффекта шлейфа
	_effectLine->posX = _position.x;
	_effectLine->posY = _position.y;
	_effectLine->Reset();
}

FPoint Cannonball::GetPosition() 
{
	return _position;
}

float Cannonball::GetScale()
{
	return _scale;
}

void Cannonball::Draw() 
{
	Render::device.PushMatrix();
	// Сдвигаем на текущую позицию
	Render::device.MatrixTranslate(math::Vector3(_position.x, _position.y, 0));
	// Поворачиваем на нужный угол
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);

	// Получаем ширину текстуры
	IRect texRect = _texture->getBitmapRect();
	FRect rect(texRect), uv(0, 1, 0, 1);
	_texture->TranslateUV(rect, uv);

	// Устанавливаем масштаб текстуры
	Render::device.MatrixScale(_scale);
	Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.0f, -texRect.height / 2.0f, 0.0f));

	// Текстура
	_texture->Bind();
	Render::DrawQuad(rect, uv);

	Render::device.PopMatrix();
}

void Cannonball::Update(float dt) 
{
	IRect texRect = _texture->getBitmapRect();
	// Высчитываем положение ядра относительно краев экрана и меняем его, 
	// чтобы получить эффект отскакивания
	if ((_position.x > MainWidget::WINDOW_WIDTH - 10) && !_didReboundMax)
	{
		_reboundCount++;
		_angle = -_angle;
		_didReboundMax = true;
		_didReboundMin = false;
	}

	if ((_position.x < 10) && !_didReboundMin)
	{
		_reboundCount++;
		_angle = -_angle;
		_didReboundMax = false;
		_didReboundMin = true;
	}

	// Подсчитываем положение ядра
	_position.x += _speed * math::cos((_angle + 90) * math::PI / 180) * dt * 10;
	_position.y += _speed * math::sin((_angle + 90) * math::PI / 180) * dt * 10;

	// Сдвигаем эффект
	_effectLine->posX = _position.x;
	_effectLine->posY = _position.y;
}

void Cannonball::KillEffect() 
{
	if (_effectLine)
	{
		_effectLine->Finish();
		_effectLine = NULL;
	}
}

int Cannonball::GetRebound()
{
	return _reboundCount;
}

void Cannonball::MakeBoom(EffectsContainer &effCont)
{
	_effectBoom = effCont.AddEffect("Boom");
	_effectBoom->posX = _position.x + 0.f;
	_effectBoom->posY = _position.y + 0.f;
	_effectBoom->Reset();
}

// --------------------------- Gun Class -------------------

Cannon::Cannon() 
{
	Init();
}

void Cannon::Init() 
{
	_cannonTexture = Core::resourceManager.Get<Render::Texture>("Cannon");
	// Масштаб
	_scale = 0.2f;
	// Начальный угол 0 градусов
	_angle = 0;
	// Начальная позиция
	_position = IPoint(MainWidget::WINDOW_WIDTH / 2, 75);

	_ballCount = 0;

	_reloading = false;
}

void Cannon::Draw() 
{
	// Рисуем ядра
	for (unsigned i = 0; i < _cannonballs.size(); i++) 
	{
		_cannonballs[i]->Draw();
	}

	// Рисуем пушку
	Render::device.PushMatrix();

	Render::device.MatrixTranslate(math::Vector3(_position.x, _position.y, 0));
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);

	IRect texRect = _cannonTexture->getBitmapRect();
	FRect rect(texRect), uv(0, 1, 0, 1);
	_cannonTexture->TranslateUV(rect, uv);

	Render::device.MatrixScale(_scale);
	Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.0f, -texRect.height / 2.0f, 0.0f));

	_cannonTexture->Bind();

	Render::DrawQuad(rect, uv);

	Render::device.PopMatrix();
}

void Cannon::DeleteBall(unsigned n) 
{
	_cannonballs[n]->KillEffect();
	delete _cannonballs[n];
	_cannonballs.erase(_cannonballs.begin() + n);
}

void Cannon::ReloadBalls()
{
	_reloadTime = 0;
	_reloading = true;
}

bool Cannon::NeedReload()
{
	return _ballCount == MAX_BALLS;
}

bool Cannon::IsReloading()
{
	return _reloading;
}

int Cannon::AmmoLeft()
{
	return MAX_BALLS - _ballCount;
}

void Cannon::Update(float dt, EffectsContainer &effCont)
{
	unsigned i = 0;
	IRect texRect = _cannonTexture->getBitmapRect();
	// Обновляем все ядра которые есть
	while (i < _cannonballs.size()) 
	{
		_cannonballs[i]->Update(dt);
		// Проверяем, если ядро застряло в отскоках или вышло за пределы экрана
		FPoint aPoint = _cannonballs[i]->GetPosition();

		if (_cannonballs[i]->GetRebound() > 3)
		{
			_cannonballs[i]->MakeBoom(effCont);
			DeleteBall(i);
		}
		else 
		if ((aPoint.y > (MainWidget::WINDOW_HEIGHT + texRect.height)) ||
			(aPoint.y < (-texRect.height)) ||
			(aPoint.x > (MainWidget::WINDOW_WIDTH + texRect.height)) ||
			(aPoint.x < (-texRect.height))) 
		{
			DeleteBall(i);
		}
		else 
		{
			i++;
		}
	}
	IPoint mouse_pos = Core::mainInput.GetMousePos();
	// Высчитываем угол наклона
	if ((mouse_pos.x - _position.x) != 0) 
	{
		_angle = math::atan((mouse_pos.y - _position.y), (mouse_pos.x - _position.x)) * 180.0f / math::PI - 90;
	}

	if (_reloading && _reloadTime > 1.0f)
	{	
		_ballCount = 0;
		_reloading = false;
	}
	_reloadTime += dt;
}

bool Cannon::MouseDown(const IPoint& mouse_pos, int speed, EffectsContainer &effCont) 
{
	if (_ballCount == MAX_BALLS)
	{
		return false;
	}
	else
	{
		_cannonballs.push_back(new Cannonball(_scale, speed, _angle, effCont));
		_ballCount++;
	}
	return true;
}

void Cannon::CheckTargetsHit(Targets *targets, EffectsContainer &effCont)
{
	for (int i = _cannonballs.size() - 1; i >= 0; i--) 
	{
		if (targets->ChekingHits(_cannonballs[i]->GetPosition(), _cannonballs[i]->GetScale(), effCont))
		{
			_cannonballs[i]->MakeBoom(effCont);
			DeleteBall(i);
		}
	}
}

void Cannon::ClearBalls()
{
	for (int i = _cannonballs.size() - 1; i >= 0; i--)
	{
		_cannonballs[i]->KillEffect();
		delete _cannonballs[i];
	}
	_cannonballs.clear();
	_ballCount = 0;
}

