#pragma once
#include "Targets.h"

#define MAX_BALLS 5

// ----------------------------- Cannonball Class ----------------------
// Снаряд для пушки
class Cannonball
{
public:
	Cannonball(float scale, float speed, float angle, EffectsContainer &effCont);

	void Draw();
	void Update(float dt);

	// получить позицию снаряда
	FPoint GetPosition();
	// Убиваем эффект
	void KillEffect();
	// Получить количество отскоков
	int GetRebound();
	// Взрыв
	void MakeBoom(EffectsContainer &effCont);
	// Размер
	float GetScale();

private:
	Render::Texture *_texture;
	FPoint _position;
	float _angle;
	float _scale;
	float _speed;
	ParticleEffectPtr _effectLine;
	ParticleEffectPtr _effectBoom;
	int _reboundCount;
	bool _didReboundMin;
	bool _didReboundMax;

	void Init();
};

// -------------------------- Cannon Class -----------------------------
class Cannon
{
public:
	Cannon();

	void Draw();
	void Update(float dt, EffectsContainer &effCont);
	// Нажатие кнопки мыши
	bool MouseDown(const IPoint& mouse_pos, int speed, EffectsContainer &effCont);
	// Удалить все ядра
	void ClearBalls();
	// Проверка попадания в мишени
	void CheckTargetsHit(Targets *targets, EffectsContainer &effCont);
	// Удаляем ядро из контейнера
	void DeleteBall(unsigned n);
	// Функции для перезарядки
	void ReloadBalls();
	bool NeedReload();
	bool IsReloading();
	int AmmoLeft();

private:
	Render::Texture *_cannonTexture;
	IPoint _position;
	float _scale;
	float _angle;
	std::vector<Cannonball *> _cannonballs;
	int _ballCount;
	bool _reloading;
	float _reloadTime;

	void Init();
};
