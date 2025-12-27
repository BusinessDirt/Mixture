import json
import graphviz
import os

def visualize_render_graph(json_file_path):
    # Check if file exists
    if not os.path.exists(json_file_path):
        print(f"Error: File '{json_file_path}' not found.")
        return

    try:
        with open(json_file_path, 'r') as f:
            data = json.load(f)
    except json.JSONDecodeError as e:
        print(f"Error decoding JSON: {e}")
        return

    # Create the directed graph
    # rankdir='LR' means "Left to Right" flow. Change to 'TB' for Top to Bottom.
    dot = graphviz.Digraph(comment='Render Graph', format='png')
    dot.attr(rankdir='LR')
    dot.attr(compound='true')
    dot.attr('node', fontname='Consolas') # Monospace font for technical details

    # --- Map Resources (ID -> Name) & Create Resource Nodes ---
    res_map = {}

    # We use a subgraph for resources to try and keep them somewhat organized,
    # though Graphviz layout engines (dot) usually handle this automatically.
    for res in data.get('resources', []):
        res_id = str(res['id'])
        res_name = res['name']
        res_map[res_id] = res_name

        # Node Style: Blue/Orange Ellipse
        dot.node(
            f"res_{res_id}",
            res_name,
            shape='ellipse',
            style='filled',
            fillcolor='#ffeebb', # Light orange
            color='#d68f15'
        )

    # --- Create Pass Nodes & Draw Edges ---
    previous_pass_id = None

    for i, pass_node in enumerate(data.get('passes', [])):
        p_id = f"pass_{pass_node['id']}"

        # --- Build the Node Label ---
        # We start with the Pass Name
        label_lines = [f"PASS {i}: {pass_node['name']}"]

        # Append Barriers if they exist
        barriers = pass_node.get('barriers', [])
        if barriers:
            label_lines.append("----------------")
            label_lines.append("[ BARRIERS ]")

            for b in barriers:
                # Resolve resource name
                r_name = res_map.get(str(b['res']), f"ID:{b['res']}")

                # Format: "TextureName: STATE_A -> STATE_B"
                barrier_str = f"{r_name}: {b['from']} -> {b['to']}"
                label_lines.append(barrier_str)

        # Join with newlines
        full_label = "\n".join(label_lines)

        # Node Style: Grey Box
        dot.node(
            p_id,
            full_label,
            shape='box',
            style='filled',
            fillcolor='#eeeeee',
            color='#333333',
            align='left' # Align text to left
        )

        # --- Draw Dependency Edges ---

        # READS (Green Arrow: Resource -> Pass)
        for read_res_id in pass_node.get('reads', []):
            res_node_id = f"res_{read_res_id}"
            dot.edge(res_node_id, p_id, color='#2e8b57', label='read')

        # WRITES (Red Arrow: Pass -> Resource)
        for write_res_id in pass_node.get('writes', []):
            res_node_id = f"res_{write_res_id}"
            dot.edge(p_id, res_node_id, color='#cd5c5c', label='write')

        # Execution Order (Invisible Edge)
        # This helps force Graphviz to lay out the passes in the order they appear in the JSON
        if previous_pass_id:
            dot.edge(previous_pass_id, p_id, style='invis', weight='10')

        previous_pass_id = p_id

    # --- Render ---
    output_filename = 'docs/visualizers/render_graph_viz'
    try:
        # This renders to 'render_graph_viz.png' and attempts to open it
        file_path = dot.render(output_filename, view=True)
        print(f"Success! Graph generated at: {file_path}")
    except Exception as e:
        print("Error rendering graph.")
        print(f"Make sure Graphviz is installed and in your PATH.\nDetails: {e}")

if __name__ == "__main__":
    # Change this filename if you exported it differently in C++
    visualize_render_graph("docs/visualizers/graph.json")
