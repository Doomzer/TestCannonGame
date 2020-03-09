#pragma once
#include "Targets.h"

#define MAX_BALLS 5

// ----------------------------- Cannonball Class ----------------------
// ������ ��� �����
class Cannonball
{
public:
	Cannonball(float scale, float speed, float angle, EffectsContainer &effCont);

	void Draw();
	void Update(float dt);

	// �������� ������� �������
	FPoint GetPosition();
	// ������� ������
	void KillEffect();
	// �������� ���������� ��������
	int GetRebound();
	// �����
	void MakeBoom(EffectsContainer &effCont);
	// ������
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
	// ������� ������ ����
	bool MouseDown(const IPoint& mouse_pos, int speed, EffectsContainer &effCont);
	// ������� ��� ����
	void ClearBalls();
	// �������� ��������� � ������
	void CheckTargetsHit(Targets *targets, EffectsContainer &effCont);
	// ������� ���� �� ����������
	void DeleteBall(unsigned n);
	// ������� ��� �����������
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
