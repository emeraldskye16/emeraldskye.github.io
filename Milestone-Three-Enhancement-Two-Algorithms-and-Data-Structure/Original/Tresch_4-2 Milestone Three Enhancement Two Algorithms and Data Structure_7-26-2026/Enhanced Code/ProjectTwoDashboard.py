# =======================================================from CRUD_Python_Module import AnimalShelter=====
# CS-499 Milestone Three Enhanced Dashboard
# Algorithms & Data Structures Category Enhancement
# Author: Emerald Tresch — 2/22/2026
# ============================================================

from dash import Dash, dcc, html
import dash_leaflet as dl
import plotly.express as px
from dash import dash_table
from dash.dependencies import Input, Output
import base64
import pandas as pd
import numpy as np

# Enhanced import using structured module path
from CRUD_Python_Module import AnimalShelter

# ============================================================
# DATABASE CONNECTION
# ============================================================

username = "aacuser"
password = "SNHU1234"

db = AnimalShelter(username, password)

# Initial load of all documents
df = pd.DataFrame.from_records(db.read({}))
df.drop(columns=['_id'], inplace=True, errors='ignore')

# ============================================================
# ALGORITHMIC ENHANCEMENTS (Milestone Three)
# ============================================================

# In-memory cache for performance-aware filtering
cache = {}

def build_query(filter_type, age_range=None, health_status=None, adoption_status=None):
    """
    Build a reusable MongoDB query based on filter parameters.
    This function demonstrates algorithmic thinking and structured data modeling.
    """
    query = {}

    rescue_map = {
        "Water": "Water",
        "Mountain": "Mountain",
        "Disaster": "Disaster"
    }

    # Breed filter using regex pattern matching
    if filter_type in rescue_map:
        query["breed"] = {"$regex": rescue_map[filter_type]}

    # Optional filters (algorithmic expansion)
    if age_range:
        query["age_upon_outcome_in_weeks"] = {
            "$gte": age_range[0],
            "$lte": age_range[1]
        }

    if health_status:
        query["health"] = health_status

    if adoption_status:
        query["outcome_type"] = adoption_status

    return query


def cached_read(query):
    """
    Cache database reads to improve performance.
    Demonstrates algorithmic efficiency and data structure usage.
    """
    key = str(query)

    if key in cache:
        return cache[key]

    results = list(db.read(query))
    cache[key] = results
    return results


def get_breed_summary(filter_type):
    """
    Use a MongoDB aggregation pipeline to summarize breeds.
    Demonstrates algorithmic data transformation and pipeline design.
    """
    query = build_query(filter_type)

    pipeline = [
        {"$match": query},
        {"$group": {"_id": "$breed", "count": {"$sum": 1}}},
        {"$sort": {"count": -1}}
    ]

    return list(db.collection.aggregate(pipeline))

# ============================================================
# DASH APP SETUP
# ============================================================

app = Dash(__name__)

# Logo
image_filename = 'Grazioso Salvare Logo.png'
encoded_image = base64.b64encode(open(image_filename, 'rb').read())

logo = html.Img(
    src='data:image/png;base64,{}'.format(encoded_image.decode()),
    style={'height': '150px'},
    id='grazioso-logo'
)

signature = html.P("Dashboard enhanced by Emerald Tresch — CS-499 Milestone Three")

app.layout = html.Div([
    html.Center(html.B(html.H1('CS-340 Dashboard — Enhanced for CS-499'))),

    html.Div(logo, style={'textAlign': 'center'}),

    signature,
    html.Hr(),

    html.Div([
        html.Label("Filter Animal Type:"),
        dcc.RadioItems(
            id='filter-type',
            options=[
                {'label': 'Water Rescue', 'value': 'Water'},
                {'label': 'Mountain Rescue', 'value': 'Mountain'},
                {'label': 'Disaster Rescue', 'value': 'Disaster'},
                {'label': 'All Animals', 'value': 'All'}
            ],
            value='All',
            inline=True
        )
    ]),

    html.Hr(),

    dash_table.DataTable(
        id='datatable-id',
        columns=[{"name": i, "id": i} for i in df.columns],
        data=df.to_dict('records'),
        editable=False,
        filter_action="native",
        sort_action="native",
        sort_mode="multi",
        row_selectable="single",
        page_action="native",
        page_current=0,
        page_size=10,
    ),

    html.Br(),
    html.Hr(),

    html.Div(className='row', style={'display': 'flex'}, children=[
        html.Div(id='graph-id', className='col s12 m6'),
        html.Div(id='map-id', className='col s12 m6')
    ])
])

# ============================================================
# CALLBACKS — INTERACTIVITY
# ============================================================

@app.callback(
    Output('datatable-id', 'data'),
    [Input('filter-type', 'value')]
)
def update_dashboard(filter_type):
    """
    Enhanced filtering using reusable query builder + caching.
    Demonstrates algorithmic efficiency and structured data processing.
    """
    query = {} if filter_type == "All" else build_query(filter_type)
    filtered_data = pd.DataFrame(cached_read(query))
    filtered_data.drop(columns=['_id'], inplace=True, errors='ignore')
    return filtered_data.to_dict('records')


@app.callback(
    Output('graph-id', "children"),
    [Input('filter-type', "value")]
)
def update_graphs(filter_type):
    """
    Enhanced pie chart using aggregation pipeline.
    Demonstrates algorithmic summarization and data transformation.
    """
    summary = get_breed_summary(filter_type)

    if not summary:
        return []

    df_summary = pd.DataFrame(summary)
    df_summary.rename(columns={'_id': 'breed'}, inplace=True)

    return [
        dcc.Graph(
            figure=px.pie(
                df_summary,
                names='breed',
                values='count',
                title='Breed Distribution (Aggregated)'
            )
        )
    ]


@app.callback(
    Output('datatable-id', 'style_data_conditional'),
    [Input('datatable-id', 'selected_columns')]
)
def update_styles(selected_columns):
    if not selected_columns:
        return []
    return [{
        'if': {'column_id': i},
        'background_color': '#D2F3FF'
    } for i in selected_columns]


@app.callback(
    Output('map-id', "children"),
    [Input('datatable-id', "derived_virtual_data"),
     Input('datatable-id', "derived_virtual_selected_rows")]
)
def update_map(viewData, index):
    """
    Enhanced geolocation map using structured data access.
    Demonstrates safe indexing and structured data handling.
    """
    if not viewData:
        return []

    dff = pd.DataFrame.from_dict(viewData)
    row = index[0] if index else 0

    if row >= len(dff):
        return []

    return [
        dl.Map(
            style={'width': '1000px', 'height': '500px'},
            center=[30.75, -97.48],
            zoom=10,
            children=[
                dl.TileLayer(id="base-layer-id"),
                dl.Marker(
                    position=[
                        dff.iloc[row]["location_lat"],
                        dff.iloc[row]["location_long"]
                    ],
                    children=[
                        dl.Tooltip(dff.iloc[row]["breed"]),
                        dl.Popup([
                            html.H1("Animal Name"),
                            html.P(dff.iloc[row]["name"])
                        ])
                    ]
                )
            ]
        )
    ]

# ============================================================
# RUN SERVER
# ============================================================

if __name__ == "__main__":
    print("""
===========================================
  OPEN YOUR DASHBOARD HERE:
  https://emptyquota-includebicycle-9090.codio.io
===========================================
""")

app.run_server(host="0.0.0.0", port=9090, debug=True)
