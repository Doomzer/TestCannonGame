#include "stdafx.h"
#include "Targets.h"
#include "MainWidget.h"

// ------------- TargetBase --------------------
TargetBase::TargetBase(FPoint position)
{
	_position = position;
	_doRebound = false;
	_dtRebound = 0;
}

void TargetBase::Draw() 
{
	// Рисуем мишень
	Render::device.PushMatrix();

	Render::device.MatrixTranslate(math::Vector3(_position.x, _position.y, 0));

	IRect texRect = _texture->getBitmapRect();
	FRect rect(texRect), uv(0, 1, 0, 1);
	_texture->TranslateUV(rect, uv);

	Render::device.MatrixScale(_scale);
	Render::device.MatrixTranslate(math::Vector3(-texRect.width / 2.0f, -texRect.height / 2.0f, 0.f));

	_texture->Bind();
	Render::DrawQuad(rect, uv);
	Render::device.PopMatrix();
}

void TargetBase::Update(float dt)
{
	IRect texRect = _texture->getBitmapRect();
	_dtRebound += dt;
	if (_doRebound && (_dtRebound > 0.2f))
	{
		_speedVector.x = -_speedVector.x;
		_speedVector.y = -_speedVector.y;
		_dtRebound = 0;
	}	
	_doRebound = false;

	// Высчитываем положение мишении относительно краев экрана и меняем его, 
	// чтобы получить эффект отскакивания
	if ((_position.x + _speedVector.x * dt) > (MainWidget::WINDOW_WIDTH - texRect.width * _scale) ||
		((_position.x + _speedVector.x * dt) < (texRect.width * _scale))) 
	{
		_speedVector.x = -_speedVector.x;
	}
	if ((_position.y + _speedVector.y * dt) > (MainWidget::WINDOW_HEIGHT - texRect.height * _scale) ||
		((_position.y + _speedVector.y * dt) < (texRect.height * _scale + 100))) 
	{
		_speedVector.y = -_speedVector.y;
	}

	_position.x += _speedVector.x * dt;
	_position.y += _speedVector.y * dt;
}

FPoint TargetBase::GetPosition()
{
	return _position;
}

float TargetBase::GetScale()
{
	return _scale;
}

FPoint TargetBase::GetVector()
{
	return _speedVector;
}

TargetBase * TargetBase::CreateTarget(int id, FPoint position)
{
	TargetBase * newTarget;
	switch (id)
	{
	case RED_ID:
		newTarget = new TargetRed(position);
		break;
	case GREEN_ID:
		newTarget = new TargetGreen(position);
		break;
	case BLUE_ID:
		newTarget = new TargetBlue(position);
		break;
	default:
		newTarget = new TargetRed(position);
	}
	return newTarget;
}

bool TargetBase::CheckColl(FPoint posBall, float scale)
{
	IRect texRect = _texture->getBitmapRect();
	float scaleToUse = scale > _scale ? scale : _scale;
	// Если точка входить в RECT текстуры,
	if (((posBall.x) >= (_position.x - texRect.width / 2 * scaleToUse)) &&
		((posBall.y) >= (_position.y - texRect.height / 2 * scaleToUse)) &&
		((posBall.x) <= (_position.x - texRect.width / 2 * scaleToUse + texRect.width / 2 * scaleToUse * 2)) &&
		((posBall.y) <= (_position.y - texRect.height / 2 * scaleToUse + texRect.height / 2 * scaleToUse * 2)))
	{
		// То ставим попадание
		return true;
	}

	return false;
}

void TargetBase::DoRebound()
{
	_doRebound = true;
}

bool TargetBase::IsDead()
{
	return _hitPoints == 0;
}

void TargetBase::WasHit()
{
	_hitPoints--;
}

// ---------------------------  Target Childs --------------------
TargetRed::TargetRed(FPoint position) : TargetBase(position)
{
	_hitPoints = 1;
	_speed = 400;
	_scale = MAX_SCALE / 2;
	_texture = Core::resourceManager.Get<Render::Texture>("Target_red");

	// Случайным образом задаем направление начальной скорости
	_speedVector.x = math::random(-1.0f, 1.0f);
	_speedVector.y = sqrt(1 - _speedVector.x * _speedVector.x);
	_speedVector.x *= _speed;
	_speedVector.y *= _speed;
}

void TargetRed::MakeBoom(EffectsContainer &effCont)
{
	ParticleEffectPtr eff = effCont.AddEffect("Boom2");
	eff->SetScale(0.75f);
	eff->posX = _position.x + 0.f;
	eff->posY = _position.y + 0.f;
	eff->Reset();
}

TargetGreen::TargetGreen(FPoint position) : TargetBase(position)
{
	_hitPoints = 2;
	_speed = 200;
	_scale = MAX_SCALE / 1.5f;
	_texture = Core::resourceManager.Get<Render::Texture>("Target_green");

	// Случайным образом задаем направление начальной скорости
	_speedVector.x = math::random(-1.0f, 1.0f);
	_speedVector.y = sqrt(1 - _speedVector.x * _speedVector.x);
	_speedVector.x *= _speed;
	_speedVector.y *= _speed;
}

void TargetGreen::MakeBoom(EffectsContainer &effCont)
{
	ParticleEffectPtr eff = effCont.AddEffect("Boom2");
	eff->SetScale(1.0f);
	eff->posX = _position.x + 0.f;
	eff->posY = _position.y + 0.f;
	eff->Reset();
}

TargetBlue::TargetBlue(FPoint position) : TargetBase(position)
{
	_hitPoints = 3;
	_speed = 100;
	_scale = MAX_SCALE;
	_texture = Core::resourceManager.Get<Render::Texture>("Target_blue");

	// Случайным образом задаем направление начальной скорости
	_speedVector.x = math::random(-1.0f, 1.0f);
	_speedVector.y = sqrt(1 - _speedVector.x * _speedVector.x);
	_speedVector.x *= _speed;
	_speedVector.y *= _speed;
}

void TargetBlue::MakeBoom(EffectsContainer &effCont)
{
	ParticleEffectPtr eff = effCont.AddEffect("Boom2");
	eff->SetScale(1.25f);
	eff->posX = _position.x + 0.f;
	eff->posY = _position.y + 0.f;
	eff->Reset();
}

// ---------------------------  Targets Class --------------------
Targets::Targets(int count, float timeLimit)
{
	_count = count;
	_timeLimit = timeLimit;

	Init();
}

void Targets::Init() 
{
	Core::Application *app = Core::appInstance;
	for (int i = 0; i < _count; i++) 
	{
		// Создаем необходимое количество мишеней
		int x = math::random(128, MainWidget::WINDOW_WIDTH - 128);
		int y = math::random(256, MainWidget::WINDOW_HEIGHT - 128);
		for (unsigned i = 0; i < _targets.size(); i++)
		{
			if (_targets[i]->CheckColl(FPoint(x, y), MAX_SCALE))
			{
				x = math::random(128, MainWidget::WINDOW_WIDTH - 128);
				y = math::random(256, MainWidget::WINDOW_HEIGHT - 128);
				i = 0;
			}
		}
		int bat;
		bat = math::random(0, 2);
		_targets.push_back(TargetBase::CreateTarget(bat,(FPoint(x, y))));
	}
}

void Targets::Draw()
{
	// Рисуем мишени
	for (unsigned i = 0; i < _targets.size(); i++) 
	{
		_targets[i]->Draw();
	}
}

int Targets::GetCountTargets()
{
	return _targets.size();
}

void Targets::CheckBallHits()
{
	for (int i = _targets.size() - 1; i >= 0; i--)
	{
		for (int j = _targets.size() - 1; j >= 0; j--)
		{
			if (i != j)
			{
				if (_targets[i]->CheckColl(_targets[j]->GetPosition(), _targets[j]->GetScale()))
				{					
					_targets[i]->DoRebound();
					break;
				}
			}
		}
	}
}

void Targets::Update(float dt)
{
	for (unsigned i = 0; i < _targets.size(); i++) 
	{
		_targets[i]->Update(dt);
	}
}

void Targets::Clear()
{
	for (int i = _targets.size() - 1; i >= 0; i--) 
	{
		delete _targets[i];
	}
	_targets.clear();
}

bool Targets::ChekingHits(FPoint posBall, float scale, EffectsContainer &effCont)
{
	bool res = false;
	for (int i = _targets.size() - 1; i >= 0; i--) 
	{
		// Проверяем попадание
		if (_targets[i]->CheckColl(posBall, scale))
		{
			res = true;
			// Проверяем ХП
			_targets[i]->WasHit();
			if (_targets[i]->IsDead())
			{
				_targets[i]->MakeBoom(effCont);
				// Удаляем мишень из вектора
				delete _targets[i];
				_targets.erase(_targets.begin() + i);
			}
			return res;
		}
	}

	return res;
}

