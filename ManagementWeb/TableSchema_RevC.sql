CREATE TABLE [dbo].[DataEmployee](
	[UserNumber] [varchar](10) NOT NULL,
	[UserName] [varchar](50) NULL,
	[UserAccount] [varchar](50) NULL,
	[Sex] [int] NULL,
	[DepartmentNO] [varchar](50) NULL,
	[CardNumber] [varchar](32) NULL,
	[IniQuota] [numeric](18, 0) NULL,
	[NowQuota] [numeric](18, 0) NULL,
	[HoldQuota] [int] NULL,
	[Role] [char](1) NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_DataEmployee] PRIMARY KEY CLUSTERED 
(
	[UserNumber] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[DataDepartment](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[DepartmentNO] [varchar](50) NULL,
	[DepartmentName] [varchar](50) NULL,
	[DepartmentOrder] [varchar](3) NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_DataDepartment] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[DataProject](
	[ProjectID] [varchar](50) NOT NULL,
	[ProjectName] [varchar](50) NULL,
	[ProjectNO] [varchar](4) NULL,
	[ProjectYear] [varchar](4) NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_DataProject] PRIMARY KEY CLUSTERED 
(
	[ProjectID] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[DataEDC](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCNO] [nvarchar](50) NULL,
	[EDCIP] [nvarchar](50) NULL,
	[EDCState] [int] NULL,
	[EDCLimitTime] [int] NULL,
	[EDCShowLimitTime] [int] NULL,
	[MachineNO] [nvarchar](50) NULL,
	[MachineIP] [varchar](50) NULL,
	[MachineOnline] [int] NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_DataEDC] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[DataPrintPay](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCNO] [nvarchar](50) NULL,
	[PrintType] [char](1) NULL,
	[PrintPay] [int] NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_DataPrintPay] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[DataLevel](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[LevelQuota] [numeric](18, 0) NULL,
	[LevelDef] [int] NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_DataLevel] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[R_EDC_Employee](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCNO] [nvarchar](50) NULL,
	[UserNO] [varchar](10) NULL,
	[StatusType] [char](1) NULL,
	[SyncFlag] [int] NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_R_EDC_Employee] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[EDCLogTmp](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCLog] [varchar](max) NULL,
 CONSTRAINT [PK_EDCLogTmp] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[EDCLog](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCLog] [varchar](max) NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_EDCLog] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[CopyCount](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCNO] [nvarchar](50) NULL,
	[ProjectNO] [varchar](4) NULL,
	[CardNumber] [varchar](32) NULL,
	[UserAccount] [varchar](50) NULL,
	[DepartmentNO] [varchar](50) NULL,
	[PrintType] [char](1) NULL,
	[PrintCount] [int] NULL,
	[UseDT] [datetime] NOT NULL,
 CONSTRAINT [PK_CopyCount] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[Sync_PQ_Employee](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[UserNumber] [varchar](10) NOT NULL,
	[StatusType] [char](1) NULL,
	[SyncFlag] [int] NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_Sync_PQ_Employee] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[Sync_EDC_Employee](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCNO] [nvarchar](50) NULL,
	[UserNumber] [varchar](10) NOT NULL,
	[StatusType] [char](1) NULL,
	[SyncFlag] [int] NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_Sync_EDC_Employee] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[Sync_PQ_EDC](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[MachineNO] [nvarchar](50) NULL,
	[MachineIP] [varchar](50) NULL,
	[StatusType] [char](1) NULL,
	[SyncFlag] [int] NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_Sync_PQ_EDC] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
--¾a¥d¸ê°T
CREATE TABLE [dbo].[EDCStatus](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCNO] [nvarchar](50) NULL,
	[CardNumber] [varchar](32) NULL,
	[ProjectNO] [varchar](4) NULL,
	[MachineNO] [nvarchar](50) NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_EDCStatus] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[PQLog](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[PQLog] [varchar](max) NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_PQLog] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
--====================================
CREATE TABLE [dbo].[Sync_EDC_Project](
	[SEQ] [numeric](18, 0) IDENTITY(1,1) NOT NULL,
	[EDCNO] [nvarchar](50) NULL,
	[ProjectNO] [varchar](4) NULL,
	[StatusType] [char](1) NULL,
	[SyncFlag] [int] NULL,
	[LatestDT] [varchar](20) NULL,
 CONSTRAINT [PK_Sync_EDC_Project] PRIMARY KEY CLUSTERED 
(
	[SEQ] ASC
)WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO

