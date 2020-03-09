#pragma once

#include <vector>

#define MAX_SCALE 0.4f

enum Targets_ID { RED_ID = 0, GREEN_ID, BLUE_ID };

//------------------------ TargetBase Class --------------------------
class TargetBase 
{
public:
	TargetBase(FPoint position);
	virtual ~TargetBase(){}
	
	void Draw();
	void Update(float dt);

	// Получить позицию
	FPoint GetPosition();
	// Проверить столкновение
	bool CheckColl(FPoint posBall, float scale);
	// Сделать отскок
	void DoRebound();
	// Обработка ХП
	bool IsDead();
	void WasHit();
	// Размер
	float GetScale();
	// Направление
	FPoint GetVector();
	// Анимация взрыва
	virtual void MakeBoom(EffectsContainer &effCont) = 0;
	// Статистический метод для создания целей
	static TargetBase* CreateTarget(int id, FPoint position);

protected:
	Render::Texture *_texture;
	FPoint _position;
	float _speed;
	float _scale;
	FPoint _speedVector;
	bool _doRebound;
	float _dtRebound;
	int _hitPoints;	
};

class TargetRed : public TargetBase
{
public:
	TargetRed(FPoint position);
	void MakeBoom(EffectsContainer &effCont) override;
};

class TargetGreen : public TargetBase
{
public:
	TargetGreen(FPoint position);
	void MakeBoom(EffectsContainer &effCont) override;
};

class TargetBlue : public TargetBase
{
public:
	TargetBlue(FPoint position);
	void MakeBoom(EffectsContainer &effCont) override;
};

//-------------------------- Targets Class ----------------
// Класс управления мишенями
class Targets
{
public:
	Targets(int count, float timeLimit);

	void Init();
	void Draw();
	void Update(float dt);

	// Проверка попадания точки в мишени
	bool ChekingHits(FPoint posBall, float scale, EffectsContainer &effCont);
	// Очистка вектора мишеней
	void Clear();
	// Получить количество мишеней в текущий момент
	int GetCountTargets();
	// Проверка на столкновение с другой мишенью
	void CheckBallHits();

private:
	std::vector <TargetBase*> _targets;
	int _count;
	float _timeLimit;
};



