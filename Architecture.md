%%{init: {
  'theme': 'dark',
  'themeVariables': {
    'fontFamily': 'monaco',
    'fontSize': '14px',
    'lineColor': '#88ccff',
    'primaryColor': '#1f2937',
    'primaryTextColor': '#88ccff',
    'primaryBorderColor': '#88ccff',
    'clusterBkg': '#111827',
    'clusterBorder': '#374151',
    'edgeLabelBackground': '#111827'
  }
}}%%

flowchart TD
    subgraph Network["Network Layer"]
        NIC["Network Interface Card"]
        KCS["Kernel Socket"]
    end

    subgraph MDFM["Market Data Feed Manager"]
        subgraph Receiver["Feed Receiver"]
            PP["Packet Processor"]
            RC["Recovery Manager"]
            SQ["Sequence Manager"]
        end

        subgraph Parser["Message Parser"]
            MP["Message Processor"]
            FP["FIX Parser"]
            NP["Native Parser"]
            DP["Data Normalizer"]
        end

        subgraph Publisher["Data Publisher"]
            PB["Publisher"]
            CB["Circular Buffer"]
            NC["Client Notifier"]
        end

        subgraph Storage["Data Storage"]
            DB[(Historical DB)]
            Cache[(In-Memory Cache)]
        end
    end

    subgraph Consumers["Data Consumers"]
        TS["Trading Strategy"]
        OB["Order Book"]
        RM["Risk Manager"]
    end

    %% Network Flow
    NIC -->|"Raw Packets"| KCS
    KCS -->|"Socket Data"| PP

    %% Receiver Flow
    PP -->|"Validated Packets"| RC
    RC -->|"Recovered Messages"| SQ
    SQ -->|"Ordered Messages"| MP

    %% Parser Flow
    MP -->|"FIX Messages"| FP
    MP -->|"Native Messages"| NP
    FP -->|"Parsed FIX"| DP
    NP -->|"Parsed Native"| DP

    %% Publisher Flow
    DP -->|"Normalized Data"| PB
    PB -->|"Published Messages"| CB
    CB -->|"Buffered Data"| NC

    %% Storage Flow
    DP -->|"Historical Data"| DB
    DP -->|"Cache Updates"| Cache

    %% Consumer Flow
    NC -->|"Market Updates"| TS
    NC -->|"Book Updates"| OB
    NC -->|"Risk Updates"| RM

classDef default fill:#1f2937,stroke:#88ccff,color:#88ccff
classDef storage fill:#374151,stroke:#88ccff,color:#88ccff
class DB,Cache storage
