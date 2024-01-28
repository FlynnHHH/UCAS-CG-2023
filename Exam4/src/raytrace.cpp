#include "framework.h"

enum MaterialType
{
	ROUGH,
	REFLECTIVE,
	REFRACTIVE
};

struct Material
{
	vec3 ka, kd, ks; // 环境光照(Ambient)，漫反射(Diffuse)，镜面反射(Specular)系数，用于phong模型计算
	float shininess; // 镜面反射强度（或者理解为物体表面的平整程度）
	vec3 F0;		 // 反射比。计算公式需要折射率n和消光系数k：[(n-1)^2+k^2]/[(n+1)^2+k^2]
	float ior;		 // 折射率（index of refraction）
	MaterialType type;
	Material(MaterialType t) { type = t; }
};

std::ostream &operator<<(std::ostream &out, const vec3 &vec)
{
	out << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
	return out;
}

std::ostream &operator<<(std::ostream &out, const Material &material)
{
	out << "MaterialType:" << material.type << '\n';
	if (material.type == ROUGH)
	{
		out << "ka:" << material.ka << ",kd:" << material.kd << ",ks:" << material.ks << '\n';
		out << "shininess:" << material.shininess << '\n';
	}
	else
	{
		out << "F0:" << material.F0 << '\n';
		out << "ior:" << material.ior << '\n';
	}
	return out;
}

struct RoughMaterial : Material
{
	RoughMaterial(vec3 _kd, vec3 _ks, float _shininess) : Material(ROUGH)
	{
		ka = _kd * M_PI;
		kd = _kd;
		ks = _ks;
		shininess = _shininess;
	}
};

vec3 operator/(vec3 num, vec3 denom)
{
	return vec3(num.x / denom.x, num.y / denom.y, num.z / denom.z);
}

struct ReflectiveMaterial : Material
{
	// n: 折射率；kappa：extinction coefficient(消光系数)
	ReflectiveMaterial(vec3 n, vec3 kappa) : Material(REFLECTIVE)
	{
		vec3 one(1, 1, 1);
		F0 = ((n - one) * (n - one) + kappa * kappa) / ((n + one) * (n + one) + kappa * kappa); // Fresnel公式
	}
};

struct RefractiveMaterial : Material
{
	// n，折射率；一个物体透明，光显然不会在其内部消逝，因此第二项忽略
	RefractiveMaterial(vec3 n) : Material(REFRACTIVE)
	{
		vec3 one(1, 1, 1);
		F0 = ((n - one) * (n - one)) / ((n + one) * (n + one));
		ior = n.x; // 该物体的折射率取单色光或取均值都可以
	}
};

struct Hit // 光线和物体表面交点
{
	float t;			   // 直线方程v=s+td。当t大于0时表示相交，默认取-1表示无交点
	vec3 position, normal; // 交点坐标，法线
	Material *material;	   // 交点处表面的材质
	Hit() { t = -1; }
};

std::ostream &operator<<(std::ostream &out, const Hit &hit)
{
	out << "hit.t:" << hit.t << '\n';
	out << "hit.position:" << hit.position << '\n';
	out << "hit.normal:" << hit.normal << '\n';
	out << *(hit.material) << '\n';
	return out;
}

struct Ray // 射线（从视角出发追踪的射线）
{
	vec3 start, dir; // 起点，方向
	Ray(vec3 _start, vec3 _dir)
	{
		start = _start;
		dir = normalize(_dir); // 方向要进行归一化，因为在光照计算时我们认为参与运算的都是归一化的向量。
	}
};

class Intersectable // 定义一个基类，可交
{
public:
	virtual Hit intersect(const Ray &ray) = 0; // 需要根据表面类型实现

protected:
	Material *material;
};

class Sphere : public Intersectable // 定义球体
{
	vec3 center;
	float radius;

public:
	Sphere(const vec3 &_center, float _radius, Material *_material)
	{
		center = _center;
		radius = _radius;
		material = _material;
	}
	~Sphere() {}

	Hit intersect(const Ray &ray)
	{
		Hit hit;
		vec3 dist = ray.start - center;	 // 距离
		float a = dot(ray.dir, ray.dir); // dot表示点乘，这里是联立光线与球面方程
		float b = dot(dist, ray.dir) * 2.0f;
		float c = dot(dist, dist) - radius * radius;
		float discr = b * b - 4.0f * a * c; // b^2-4ac
		if (discr < 0)						// 无交点
			return hit;
		float sqrt_discr = sqrtf(discr);
		float t1 = (-b + sqrt_discr) / 2.0f / a; // 求得两个交点，t1 >= t2
		float t2 = (-b - sqrt_discr) / 2.0f / a;
		if (t1 <= 0)
			return hit;
		hit.t = (t2 > 0) ? t2 : t1; // 取近的那个交点
		hit.position = ray.start + ray.dir * hit.t;
		hit.normal = (hit.position - center) / radius;
		hit.material = material;
		return hit;
	}
};

class Plane : public Intersectable
{				 // 点法式方程表示平面
	vec3 normal; // 法线
	vec3 p0;	 // 线上一点坐标，N(p-p0)=0
public:
	Plane(vec3 _p0, vec3 _normal, Material *_material)
	{
		normal = normalize(_normal);
		p0 = _p0;
		material = _material;
	}

	Hit intersect(const Ray &ray)
	{
		Hit hit;
		float nD = dot(ray.dir, normal); // 射线方向与法向量点乘，为0表示平行
		if (nD == 0)
			return hit;

		float t1 = (dot(normal, p0) - dot(normal, ray.start)) / nD;
		if (t1 < 0)
			return hit;
		hit.t = t1;
		hit.position = ray.start + ray.dir * hit.t;
		hit.normal = normal;
		hit.material = material;
		return hit;
	}
};

bool operator==(const vec3 &n1, const vec3 &n2)
{
	if (n1.x == n2.x && n1.y == n2.y && n1.z == n2.z)
	{
		return true;
	}
	return false;
}

class Cylinder : public Intersectable
{ // 无(有)限长圆柱面，(q-pa-(va,q-pa)va)^2-r^2=0，q是面上一点
	// (va,q-pa)，是点乘
	vec3 va;	  // 转轴方向
	vec3 pa;	  // 转轴中心点（或者理解成某一截面的中心）
	float radius; // 旋转半径
	vec3 p1;	  // 下底面圆心，有(va,q-p1)>0，这个参数不给就是无限长圆柱
	vec3 p2;	  // 上底面圆心，有(va,q-p2)<0，这个参数不给就是无限长圆柱

public:
	Cylinder(vec3 _pa, vec3 _va, float _radius, Material *_material, vec3 _p1 = vec3(0, 0, 0), vec3 _p2 = vec3(0, 0, 0))
	{
		pa = _pa;
		va = normalize(_va);
		radius = _radius;
		material = _material;
		p1 = _p1; // 如果为(0,0,0)则说明用户定义无限长圆柱面
		p2 = _p2;
	}

	Hit intersect(const Ray &ray)
	{
		Hit hit;
		// 把直线方程 q = s + dt 代入，则
		// (s - pa + dt - (va, s - pa + dt)va)^2 - r^2 = 0
		// t为未知量，形式为At^2+Bt+C=0
		// A = (d - (d,va)va)^2
		// B = 2(d - (d,va)va, Δp-(Δp,va)va)
		// C = (Δp - (Δp,va)va)^2 - r^2
		// Δp = s - pa
		vec3 A_operand = ray.dir - dot(ray.dir, va) * va;
		float A = dot(A_operand, A_operand);
		vec3 delta_p = ray.start - pa;
		vec3 B_operand = delta_p - dot(delta_p, va) * va;
		float B = 2 * dot(A_operand, B_operand);
		float C = dot(B_operand, B_operand) - radius * radius;
		float discr = B * B - 4 * A * C;
		if (discr < 0)
			return hit;

		float sqrt_discr = sqrtf(discr);
		float t1 = (-B + sqrt_discr) / 2 * A; // t1>=t2
		float t2 = (-B - sqrt_discr) / 2 * A;

		if (p1 == vec3(0, 0, 0) || p2 == vec3(0, 0, 0))
		{
			if (t1 < 0)
				return hit;

			hit.t = (t2 > 0) ? t2 : t1; // 取近的那个交点
			hit.position = ray.start + ray.dir * hit.t;
			vec3 N = hit.position - pa - dot(va, hit.position - pa) * va;
			hit.normal = normalize(N);
			hit.material = material;
			// cout << hit << '\n';
			return hit;
		}
		else
		{
			std::vector<float> t_candidate;
			// 对于有限圆柱还要看交点是不是在上下底之间
			// 另外要判断上下底是否有交点

			// 第一步，判断侧面的两个交点是否有效（在上下底之间）
			vec3 q1 = ray.start + ray.dir * t1;
			vec3 q2 = ray.start + ray.dir * t2;
			if (t1 >= 0 && dot(va, q1 - p1) > 0 && dot(va, q1 - p2) < 0)
				t_candidate.push_back(t1);

			if (t2 >= 0 && dot(va, q2 - p1) > 0 && dot(va, q2 - p2) < 0)
				t_candidate.push_back(t2);

			// 第二步，求与上下底面的交点
			// 把直线公式代入下底面为：
			// (va,d)t + (va,s) - (va,p1) = 0
			float vad = dot(va, ray.dir);
			float vas = dot(va, ray.start);
			float vap1 = dot(va, p1);
			float t_bottom = (vap1 - vas) / vad;
			if (t_bottom >= 0)
			{
				vec3 q3 = ray.start + ray.dir * t_bottom;
				if (dot(q3 - p1, q3 - p1) < radius * radius)
					t_candidate.push_back(t_bottom);
			}

			// 把直线公式代入上底面，则
			float vap2 = dot(va, p2);
			float t_up = (vap2 - vas) / vad;
			if (t_up >= 0)
			{
				vec3 q4 = ray.start + ray.dir * t_up;
				if (dot(q4 - p2, q4 - p2) < radius * radius)
					t_candidate.push_back(t_up);
			}

			if (t_candidate.size() == 0)
				return hit;

			// 遍历所有候选t，找到最小的
			float mint = 0x3FFFFFFF;
			for (auto it = t_candidate.begin(); it != t_candidate.end(); it++)
			{
				if (*it < mint)
					mint = *it;
			}

			hit.t = mint; // 取近的那个交点
			hit.position = ray.start + ray.dir * hit.t;
			if (mint == t1 || mint == t2)
			{
				vec3 N = hit.position - pa - dot(va, hit.position - pa) * va;
				hit.normal = normalize(N);
			}
			else if (mint == t_up)
				hit.normal = va;
			else if (mint == t_bottom)
				hit.normal = -va;
			hit.material = material;
		}
		return hit;
	}
};

class Cube : public Intersectable // 定义立方体
{
	vec3 vertexmin;
	vec3 vertexmax; // 相对的两个顶点的坐标确定一个立方体(假设立方体的轴与坐标轴平行)

public:
	Cube(vec3 _vertexmin, vec3 _vertexmax, Material *_material)
	{
		vertexmin = _vertexmin;
		vertexmax = _vertexmax;
		material  = _material;
	}
	~Cube() {}

	Hit intersect(const Ray &ray)
	{
		Hit hit;
		float txnear = (vertexmin.x - ray.start.x) / ray.dir.x;
		float txfar  = (vertexmax.x - ray.start.x) / ray.dir.x;
		float tynear = (vertexmin.y - ray.start.y) / ray.dir.y;
		float tyfar  = (vertexmax.y - ray.start.y) / ray.dir.y;
		float tznear = (vertexmin.z - ray.start.z) / ray.dir.z;
		float tzfar  = (vertexmax.z - ray.start.z) / ray.dir.z;
		float tmin   = max(max(min(txnear, txfar), min(tynear, tyfar)), min(tznear, tzfar));
		float tmax   = min(min(max(txnear, txfar), max(tynear, tyfar)), max(tznear, tzfar));
		if (tmin <= tmax) // 相交
		{
			hit.t = tmin;
			hit.position = ray.start + ray.dir * hit.t;
			if (hit.position.x == vertexmin.x)
				hit.normal = vec3(-1, 0, 0);
			else if (hit.position.x == vertexmax.x)
				hit.normal = vec3(1, 0, 0);
			else if (hit.position.y == vertexmin.y)
				hit.normal = vec3(0, -1, 0);
			else if (hit.position.y == vertexmax.y)
				hit.normal = vec3(0, 1, 0);
			else if (hit.position.z == vertexmin.z)
				hit.normal = vec3(0, 0, -1);
			else if (hit.position.z == vertexmax.z)
				hit.normal = vec3(0, 0, 1);
			hit.material = material;
		}
		return hit;
	}
};

class Camera
{								 // 用相机表示用户视线
	vec3 eye, lookat, right, up; // eye用来定义用户位置；lookat(视线中心)，right和up共同定义了视窗大小
	float fov;

public:
	void set(vec3 _eye, vec3 _lookat, vec3 _up, float _fov) // fov视场角
	{
		eye = _eye;
		lookat = _lookat;
		fov = _fov;
		vec3 w = eye - lookat;
		float windowSize = length(w) * tanf(fov / 2);
		right = normalize(cross(_up, w)) * windowSize; // 要确保up、right与eye到lookat的向量垂直(所以叉乘)
		up = normalize(cross(w, right)) * windowSize;
	}

	Ray getRay(int X, int Y)
	{
		vec3 dir = lookat + right * (2 * (X + 0.5f) / windowWidth - 1) + up * (2 * (Y + 0.5f) / windowHeight - 1) - eye;
		return Ray(eye, dir);
	}

	void Animate(float dt) // 修改eye的位置（旋转）
	{
		vec3 d = eye - lookat;
		eye = vec3(d.x * cos(dt) + d.z * sin(dt), d.y, -d.x * sin(dt) + d.z * cos(dt)) + lookat;
		set(eye, lookat, up, fov);
	}
};

struct Light
{ // 定义光源
	vec3 direction;
	vec3 Le; // 光照强度
	Light(vec3 _direction, vec3 _Le)
	{
		direction = _direction;
		Le = _Le;
	}
};

const float epsilon = 0.0001f;

class Scene
{ // 场景，物品和光源集合
	std::vector<Intersectable *> objects;
	std::vector<Light *> lights;
	Camera camera;
	vec3 La; // 环境光
public:
	void build()
	{
		vec3 eye = vec3(0, 0, 4), vup = vec3(0, 1, 0), lookat = vec3(0, 0, 0);
		float fov = 45 * M_PI / 180;
		camera.set(eye, lookat, vup, fov);

		La = vec3(0.4f, 0.4f, 0.4f);
		vec3 lightDirection(1, 1, 1), Le(2, 2, 2);
		lights.push_back(new Light(lightDirection, Le));

		vec3 ks(2, 2, 2);

		
		objects.push_back(new Cube(vec3(0, 0, 0.3), vec3(0.3, 0.6, 0.9), new RoughMaterial(vec3(0.3, 0.2, 0.1), ks, 50)));  // 漫反射立方体

		// objects.push_back(new Sphere(vec3(0.55, 0, 0), 0.5, new RoughMaterial(vec3(0.1, 0.2, 0.3), ks, 100)));  // 偏蓝色

		// objects.push_back(new Sphere(vec3(0, 0.5, -0.8), 0.5, new RoughMaterial(vec3(0.3, 0, 0.2), ks, 20)));	 // 偏粉色

		objects.push_back(new Sphere(vec3(0, 0.3, 0.6), 0.3, new RefractiveMaterial(vec3(1.5, 1.5, 1.5))));

		// objects.push_back(new Sphere(vec3(0, -6.5, 0), 6, new ReflectiveMaterial(vec3(0.14, 0.16, 0.13), vec3(4.1, 2.3, 3.1))));	// 数据来源于“银”
		
		objects.push_back(new Sphere(vec3(-0.95, 0, 0), 0.3, new ReflectiveMaterial(vec3(0.14, 0.16, 0.13), vec3(4.1, 2.3, 3.1)))); // 镜面反射球

		// objects.push_back(new Cylinder(vec3(0.55,0,0), vec3(0,1,0), 0.5f, new RoughMaterial(vec3(0.1, 0.2, 0.3), ks, 100)));	  // 无限长圆柱

		objects.push_back(new Sphere(vec3(0, 0.5, -0.8), 0.3, new RoughMaterial(vec3(0.3, 0, 0.2), ks, 20))); // 漫反射球

		objects.push_back(new Cylinder(vec3(0.55, 0, 0), vec3(0, 1, 0), 0.4f, new RoughMaterial(vec3(0.1, 0.2, 0.3), ks, 100), vec3(0.55, -0.1, 0), vec3(0.55, 0.4, 0))); // 有限长圆柱

		objects.push_back(new Plane(vec3(0, -0.3, 0), vec3(0, 1, 0), new ReflectiveMaterial(vec3(0.14, 0.16, 0.13), vec3(4.1, 2.3, 3.1))));
	}

	void render(std::vector<vec4> &image)
	{
		long timeStart = glutGet(GLUT_ELAPSED_TIME);
		for (int Y = 0; Y < windowHeight; Y++) // 对视窗的每一个像素做渲染
		{
			for (int X = 0; X < windowWidth; X++)
			{
				vec3 color = trace(camera.getRay(X, Y)); // 追踪这条光线，获得返回的颜色
				image[Y * windowWidth + X] = vec4(color.x, color.y, color.z, 1);
			}
		}

		cout << "Rendering time:" << glutGet(GLUT_ELAPSED_TIME) - timeStart << " milliseconds" << '\n';
	}

	Hit firstIntersect(Ray ray) // 求最近的交点
	{
		Hit bestHit;
		for (Intersectable *object : objects)
		{
			Hit hit = object->intersect(ray);
			if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t))
				bestHit = hit;
		}
		if (dot(ray.dir, bestHit.normal) > 0)
			bestHit.normal = bestHit.normal * (-1);
		return bestHit;
	}

	bool shadowIntersect(Ray ray) // 该射线在指向光源的路径上是否与其他物体有交
	{
		for (Intersectable *object : objects)
			if (object->intersect(ray).t > 0)
				return true;
		return false;
	}

	vec3 trace(Ray ray, int depth = 0) // 光线追踪
	{
		if (depth > 5) // 设置迭代上限5次
			return La;
		Hit hit = firstIntersect(ray);
		if (hit.t < 0) // 不再有交，则返回环境光即可
			return La;

		if (hit.material->type == ROUGH)
		{
			vec3 outRadiance = hit.material->ka * La; // 初始化返回光线（或者说阴影）
			for (Light *light : lights)
			{
				Ray shadowRay(hit.position + hit.normal * epsilon, light->direction);
				float cosTheta = dot(hit.normal, light->direction);
				if (cosTheta > 0) // 如果cos小于0（钝角），说明光找到的是物体背面，用户看不到
				{
					if (!shadowIntersect(shadowRay)) // 如果与其他物体有交，则处于阴影中；反之按Phong模型计算
					{
						outRadiance = outRadiance + light->Le * hit.material->kd * cosTheta;
						vec3 halfway = normalize(-ray.dir + light->direction);
						float cosDelta = dot(hit.normal, halfway);
						if (cosDelta > 0)
							outRadiance = outRadiance + light->Le * hit.material->ks * powf(cosDelta, hit.material->shininess);
					}
				}
			}
			return outRadiance;
		}

		float cosa = -dot(ray.dir, hit.normal); // 镜面反射（继续追踪）
		vec3 one(1, 1, 1);
		vec3 F = hit.material->F0 + (one - hit.material->F0) * pow(1 - cosa, 5);
		vec3 reflectedDir = ray.dir - hit.normal * dot(hit.normal, ray.dir) * 2.0f; // 反射光线R = v + 2Ncosa
		vec3 outRadiance = trace(Ray(hit.position + hit.normal * epsilon, reflectedDir), depth + 1) * F;

		if (hit.material->type == REFRACTIVE) // 对于透明物体，计算折射（继续追踪）
		{
			float disc = 1 - (1 - cosa * cosa) / hit.material->ior / hit.material->ior;
			if (disc >= 0)
			{
				vec3 refractedDir = ray.dir / hit.material->ior + hit.normal * (cosa / hit.material->ior - sqrt(disc));
				outRadiance = outRadiance + trace(Ray(hit.position - hit.normal * epsilon, refractedDir), depth + 1) * (one - F);
			}
		}
		return outRadiance;
	}

	void Animate(float dt)
	{
		camera.Animate(dt);
	}
};

Scene scene;
GPUProgram gpuProgram; // vertex and fragment shaders

// vertex shader in GLSL
const char *vertexSource = R"(
	#version 330
	precision highp float;

	layout(location = 0) in vec2 cVertexPosition;	// Attrib Array 0
	out vec2 texcoord;

	void main(){
		texcoord = (cVertexPosition + vec2(1,1))/2;						// -1,1 to 0,1
		gl_Position = vec4(cVertexPosition.x, cVertexPosition.y, 0, 1);	// transform to clipping space
	}
)";

// fragment shader in GLSL
const char *fragmentSource = R"(
	#version 330
	precision highp float;

	uniform sampler2D textureUnit;
	in vec2 texcoord;			// interpolated texture coordinates
	out vec4 fragmentColor;		// output that goes to the raster memory as told by glBindFragDataLocation

	void main(){
		fragmentColor = texture(textureUnit, texcoord);
	}
)";

class FullScreenTexturedQuad
{
	unsigned int vao = 0, textureId = 0; // vertex array object id and texture id
public:
	FullScreenTexturedQuad(int windowWidth, int windowHeight)
	{
		glGenVertexArrays(1, &vao); // create 1 vertex array object
		glBindVertexArray(vao);		// make it active

		unsigned int vbo;	   // vertex buffer objects
		glGenBuffers(1, &vbo); // Generate 1 vertex buffer objects

		// vertex coordinates: vbo0 -> Attrib Array 0 -> vertexPosition of the vertex shader
		glBindBuffer(GL_ARRAY_BUFFER, vbo);					 // make it active, it is an array
		float vertexCoords[] = {-1, -1, 1, -1, 1, 1, -1, 1}; // two traiangles forming a quad
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glGenTextures(1, &textureId);									  // id generation
		glBindTexture(GL_TEXTURE_2D, textureId);						  // binding
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // sampling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void LoadTexture(std::vector<vec4> &image)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, &image[0]); // To GPU
	}

	void Draw()
	{
		glBindVertexArray(vao); // make the vao and its vbos active playing the role of the data source
		int location = glGetUniformLocation(gpuProgram.getId(), "textureUnit");
		const unsigned int textureUnit = 0;
		if (location >= 0)
		{
			glUniform1i(location, textureUnit);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, textureId);
		}
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // draw two triangles forming a quad
	}
};

FullScreenTexturedQuad *fullScreenTextureQuad;

// Initialization, create an OpenGL context
void onInitialization()
{
	glViewport(0, 0, windowWidth, windowHeight);
	scene.build();
	fullScreenTextureQuad = new FullScreenTexturedQuad(windowWidth, windowHeight);
	gpuProgram.create(vertexSource, fragmentSource, "fragmentColor"); // create program for the GPU
}

// Window has become invalid: Redraw
void onDisplay()
{
	std::vector<vec4> image(windowWidth * windowHeight);
	scene.render(image);
	fullScreenTextureQuad->LoadTexture(image);
	fullScreenTextureQuad->Draw();
	glutSwapBuffers();
}

void onKeyboard(unsigned char key, int pX, int pY) {}

void onKeyboardUp(unsigned char key, int pX, int pY) {}

void onMouse(int button, int state, int pX, int pY) {}

void onMouseMotion(int pX, int pY) {}

void onIdle()
{
	scene.Animate(0.1f);
	glutPostRedisplay();
}