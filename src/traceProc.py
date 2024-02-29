import json

def getThreadNameByTid(id):
    agentName = [
        'TCC Agent',
        'UM2UM Agent',
        'CVU Agent',
        'SV Agent',
        'Fence Agent',
        'Barrier Agent',
        'VCS Agent',
        'ICB Test Agent',
        'HIB DMA Agent',
        'TCC Agent'
    ]
    return agentName[id]



def generate_tcc_json():
    events = []
    # 生成32个进程，每个进程ID从12到43
    for pid in range(1, 48):
        # 添加进程名称事件
        events.append({
            "name": "process_name",
            "ph": "M",
            "pid": pid,
            "args": {"name": f"TileID: {pid}"}
        })

        # 为每个进程生成6个线程
        for tid in range(1, 10):
            # 添加线程名称事件
            events.append({
                "name": "thread_name",
                "ph": "M",
                "pid": pid,
                "tid": tid,
                "args": {"name": f" {getThreadNameByTid(tid)}"}
            })
    return events


# 定义一个函数来处理每个日志块
def parse_log_block(block):
    events = []
    lines = block.strip().split("\n")
    header = lines[0]  # 第一行包含Tile ID和AgentType
    # 解析Tile ID和AgentType
    _,_, tile_id, _, agent_type = header.split()
    tile_id_int = int(tile_id)
    agent_type_int = int(agent_type)
    if agent_type_int == 0: 
        agent_type_int = 9
    if agent_type_int == 8 or agent_type_int == 5:
        tile_id_int = 1
    name = f"Tile ID: {tile_id} AgentType: {agent_type}"
    cat = f"AgentType: {agent_type}"

    for line in lines[1:]:
        if line.strip() == "===============================":
            break  # 结束标志
        start_ts, end_ts = line.split()
        events.append({"name": name, "cat": cat, "ph": "B", "ts": int(start_ts), "pid": tile_id_int, "tid": agent_type_int})
        events.append({"name": name, "cat": cat, "ph": "E", "ts": int(end_ts), "pid": tile_id_int, "tid": agent_type_int})

    return events

# 读取文件并解析每个日志块
def parse_log_file(filepath):
    with open(filepath, 'r') as file:
        content = file.read()
    
    # 分割日志为多个块，每个块对应一个AgentType
    blocks = content.split("===============================")
    all_events = []
    print(len(blocks))
    for block in blocks:
        if block.strip():  # 忽略空块
            events = parse_log_block(block)
            all_events.extend(events) 

    return all_events

# 生成Trace Event Format的JSON结构
def generate_tef_json(events):
    allEvent= generate_tcc_json()
    allEvent.extend(events)
    tef = {
        "traceEvents": allEvent
    }
    return json.dumps(tef, indent=2)

# 主函数
def main():
    filepath = 'agentAll19.txt'  # 假设日志文件名为log_data.txt
    events = parse_log_file(filepath)
    tef_json = generate_tef_json(events)
    # 打印到控制台，或者写入到文件
    output_filepath = 'AllAgentWithHibDma5TCCNoCalcBias.json'
    with open(output_filepath, 'w') as f:
        f.write(tef_json)
    print(f"Trace Event Format JSON has been written to {output_filepath}")

if __name__ == "__main__":
    main()
