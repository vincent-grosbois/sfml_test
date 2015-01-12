#pragma once


#include "DetourNavMesh.h"
#include "DetourCrowd.h"
#include "Recast.h"
#include "utils/Array2D.h"



class NavMeshGenerator
{
protected:
	class dtNavMesh* m_navMesh;
	class dtNavMeshQuery* m_navQuery;
	class dtCrowd* m_crowd;

	unsigned char m_navMeshDrawFlags;

	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	float m_edgeMaxLen;
	float m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailSampleDist;
	float m_detailSampleMaxError;
	int m_partitionType;


	bool m_keepInterResults;
	float m_totalBuildTimeMs;

	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcConfig m_cfg;	
	rcPolyMeshDetail* m_dmesh;
	rcContext* m_ctx;
	
	void cleanup();
		
public:
	
	class dtNavMesh* getNavMesh() { return m_navMesh; }
	class dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
	class dtCrowd* getCrowd() { return m_crowd; }
	float getAgentRadius() { return m_agentRadius; }
	float getAgentHeight() { return m_agentHeight; }
	float getAgentClimb() { return m_agentMaxClimb; }

	
	inline unsigned char getNavMeshDrawFlags() const { return m_navMeshDrawFlags; }
	inline void setNavMeshDrawFlags(unsigned char flags) { m_navMeshDrawFlags = flags; }

	NavMeshGenerator(rcContext* ctx);
	~NavMeshGenerator();

	
	bool handleBuild(const Array2D<int>& tab);
};

/// Recast build context.
class BuildContext : public rcContext
{
	//TimeVal m_startTime[RC_MAX_TIMERS];
	int m_accTime[RC_MAX_TIMERS];

	static const int MAX_MESSAGES = 1000;
	const char* m_messages[MAX_MESSAGES];
	int m_messageCount;
	static const int TEXT_POOL_SIZE = 8000;
	char m_textPool[TEXT_POOL_SIZE];
	int m_textPoolSize;
	
public:
	BuildContext();
	virtual ~BuildContext();
	
	/// Dumps the log to stdout.
	void dumpLog(const char* format, ...);
	/// Returns number of log messages.
	int getLogCount() const;
	/// Returns log message text.
	const char* getLogText(const int i) const;
	
protected:	
	/// Virtual functions for custom implementations.
	///@{
	virtual void doResetLog();
	virtual void doLog(const rcLogCategory /*category*/, const char* /*msg*/, const int /*len*/);
	virtual void doResetTimers();
	virtual void doStartTimer(const rcTimerLabel /*label*/);
	virtual void doStopTimer(const rcTimerLabel /*label*/);
	virtual int doGetAccumulatedTime(const rcTimerLabel /*label*/) const;
	///@}
};



////////////
// Tool to create crowds.

struct CrowdToolParams
{
	bool m_expandSelectedDebugDraw;
	bool m_showCorners;
	bool m_showCollisionSegments;
	bool m_showPath;
	bool m_showVO;
	bool m_showOpt;
	bool m_showNeis;
	
	bool m_expandDebugDraw;
	bool m_showLabels;
	bool m_showGrid;
	bool m_showNodes;
	bool m_showPerfGraph;
	bool m_showDetailAll;
	
	bool m_expandOptions;
	bool m_anticipateTurns;
	bool m_optimizeVis;
	bool m_optimizeTopo;
	bool m_obstacleAvoidance;
	float m_obstacleAvoidanceType;
	bool m_separation;
	float m_separationWeight;
};
struct OverWorldDisplay;

class CrowdToolState 
{
	NavMeshGenerator* m_sample;
	dtNavMesh* m_nav;
	dtCrowd* m_crowd;
	
	float m_targetPos[3];
	dtPolyRef m_targetRef;

	dtCrowdAgentDebugInfo m_agentDebug;
	dtObstacleAvoidanceDebugData* m_vod;
	
	static const int MAX_AGENTS = 128;
	

	CrowdToolParams m_toolParams;

	bool m_run;

public:
	CrowdToolState();
	~CrowdToolState();
	
	void init(class NavMeshGenerator* sample);
	void reset();
	void handleUpdate(const float dt);

	inline bool isRunning() const { return m_run; }
	inline void setRunning(const bool s) { m_run = s; }
	
	void addAgent(const float* pos);
	void removeAgent(const int idx);
	void hilightAgent(const int idx);
	void updateAgentParams();
	int hitTestAgents(const float* s, const float* p);
	void setMoveTarget(const float* p, bool adjust);
	void updateTick(const float dt);

	inline CrowdToolParams* getToolParams() { return &m_toolParams; }

	void handleRender(OverWorldDisplay& owd);
};

class CrowdTool
{
	NavMeshGenerator* m_sample;
	CrowdToolState* m_state;
	
	enum ToolMode
	{
		TOOLMODE_CREATE,
		TOOLMODE_MOVE_TARGET,
		TOOLMODE_SELECT,
		TOOLMODE_TOGGLE_POLYS,
	};
	ToolMode m_mode;
	
	void updateAgentParams();
	void updateTick(const float dt);
	
public:
	CrowdTool();
	~CrowdTool();
	
	int type() { return 0; }
	void init(NavMeshGenerator* sample);
};
