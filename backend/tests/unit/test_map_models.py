"""
Unit tests for Map Models
"""

import pytest
from datetime import datetime
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from sqlalchemy.pool import StaticPool

from app.models.map import Map, MapSession, RobotPosition, RFIDTag, LocalizationConfig
from app.models.sensors import SensorConfiguration, SensorReading, SensorStatus, SensorCalibration
from app.core.database import Base


class TestMapModels:
    """Test cases for Map models"""
    
    @pytest.fixture
    def db_session(self):
        """Create in-memory database session for testing"""
        engine = create_engine(
            "sqlite:///:memory:",
            connect_args={"check_same_thread": False},
            poolclass=StaticPool,
        )
        Base.metadata.create_all(engine)
        SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
        session = SessionLocal()
        yield session
        session.close()
    
    def test_map_creation(self, db_session):
        """Test Map model creation"""
        # Create map
        map_data = Map(
            map_id="test_map_001",
            name="Test Map",
            resolution=50.0,
            width=1000,
            height=1000,
            occupancy_grid=[[-1, 0, 1], [0, 1, 0]],
            robot_trajectory=[],
            rfid_positions=[],
            metadata={"test": True}
        )
        
        db_session.add(map_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_map = db_session.query(Map).filter(Map.map_id == "test_map_001").first()
        
        assert retrieved_map is not None
        assert retrieved_map.map_id == "test_map_001"
        assert retrieved_map.name == "Test Map"
        assert retrieved_map.resolution == 50.0
        assert retrieved_map.width == 1000
        assert retrieved_map.height == 1000
        assert retrieved_map.occupancy_grid == [[-1, 0, 1], [0, 1, 0]]
        assert retrieved_map.robot_trajectory == []
        assert retrieved_map.rfid_positions == []
        assert retrieved_map.metadata == {"test": True}
        assert retrieved_map.is_active is True
        assert retrieved_map.created_at is not None
        assert retrieved_map.updated_at is not None
    
    def test_map_session_creation(self, db_session):
        """Test MapSession model creation"""
        # Create map first
        map_data = Map(
            map_id="test_map_001",
            name="Test Map",
            resolution=50.0,
            width=1000,
            height=1000,
            occupancy_grid=[[-1, 0, 1]],
            robot_trajectory=[],
            rfid_positions=[]
        )
        db_session.add(map_data)
        db_session.commit()
        
        # Create session
        session_data = MapSession(
            session_id="test_session_001",
            map_id="test_map_001",
            total_scans=10,
            mapping_quality=0.8
        )
        
        db_session.add(session_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_session = db_session.query(MapSession).filter(
            MapSession.session_id == "test_session_001"
        ).first()
        
        assert retrieved_session is not None
        assert retrieved_session.session_id == "test_session_001"
        assert retrieved_session.map_id == "test_map_001"
        assert retrieved_session.total_scans == 10
        assert retrieved_session.mapping_quality == 0.8
        assert retrieved_session.is_active is True
        assert retrieved_session.start_time is not None
        assert retrieved_session.end_time is None
        assert retrieved_session.created_at is not None
    
    def test_robot_position_creation(self, db_session):
        """Test RobotPosition model creation"""
        # Create map and session first
        map_data = Map(
            map_id="test_map_001",
            name="Test Map",
            resolution=50.0,
            width=1000,
            height=1000,
            occupancy_grid=[[-1, 0, 1]],
            robot_trajectory=[],
            rfid_positions=[]
        )
        db_session.add(map_data)
        
        session_data = MapSession(
            session_id="test_session_001",
            map_id="test_map_001"
        )
        db_session.add(session_data)
        db_session.commit()
        
        # Create robot position
        position_data = RobotPosition(
            map_id="test_map_001",
            session_id="test_session_001",
            x=100.0,
            y=200.0,
            theta=1.57,
            confidence=0.9,
            source="hybrid"
        )
        
        db_session.add(position_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_position = db_session.query(RobotPosition).filter(
            RobotPosition.map_id == "test_map_001"
        ).first()
        
        assert retrieved_position is not None
        assert retrieved_position.map_id == "test_map_001"
        assert retrieved_position.session_id == "test_session_001"
        assert retrieved_position.x == 100.0
        assert retrieved_position.y == 200.0
        assert retrieved_position.theta == 1.57
        assert retrieved_position.confidence == 0.9
        assert retrieved_position.source == "hybrid"
        assert retrieved_position.timestamp is not None
    
    def test_rfid_tag_creation(self, db_session):
        """Test RFIDTag model creation"""
        # Create RFID tag
        rfid_data = RFIDTag(
            rfid_id="RFID_001",
            position_x=100.0,
            position_y=200.0,
            orientation=1.57,
            description="Test RFID tag"
        )
        
        db_session.add(rfid_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_rfid = db_session.query(RFIDTag).filter(
            RFIDTag.rfid_id == "RFID_001"
        ).first()
        
        assert retrieved_rfid is not None
        assert retrieved_rfid.rfid_id == "RFID_001"
        assert retrieved_rfid.position_x == 100.0
        assert retrieved_rfid.position_y == 200.0
        assert retrieved_rfid.orientation == 1.57
        assert retrieved_rfid.description == "Test RFID tag"
        assert retrieved_rfid.is_active is True
        assert retrieved_rfid.created_at is not None
        assert retrieved_rfid.updated_at is not None
    
    def test_sensor_data_creation(self, db_session):
        """Test SensorData model creation"""
        # Create sensor data
        sensor_data = SensorData(
            sensor_type="rfid",
            sensor_id="RFID_001",
            data={"rfid_id": "RFID_001", "signal_strength": 0.8},
            quality=0.9
        )
        
        db_session.add(sensor_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_sensor = db_session.query(SensorData).filter(
            SensorData.sensor_id == "RFID_001"
        ).first()
        
        assert retrieved_sensor is not None
        assert retrieved_sensor.sensor_type == "rfid"
        assert retrieved_sensor.sensor_id == "RFID_001"
        assert retrieved_sensor.data == {"rfid_id": "RFID_001", "signal_strength": 0.8}
        assert retrieved_sensor.quality == 0.9
        assert retrieved_sensor.is_valid is True
        assert retrieved_sensor.timestamp is not None
    
    def test_localization_config_creation(self, db_session):
        """Test LocalizationConfig model creation"""
        # Create localization config
        config_data = LocalizationConfig(
            config_key="rfid_threshold",
            config_value={"threshold": 0.5, "timeout": 1000},
            config_type="rfid",
            description="RFID signal threshold configuration"
        )
        
        db_session.add(config_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_config = db_session.query(LocalizationConfig).filter(
            LocalizationConfig.config_key == "rfid_threshold"
        ).first()
        
        assert retrieved_config is not None
        assert retrieved_config.config_key == "rfid_threshold"
        assert retrieved_config.config_value == {"threshold": 0.5, "timeout": 1000}
        assert retrieved_config.config_type == "rfid"
        assert retrieved_config.description == "RFID signal threshold configuration"
        assert retrieved_config.is_active is True
        assert retrieved_config.created_at is not None
        assert retrieved_config.updated_at is not None


class TestSensorModels:
    """Test cases for Sensor models"""
    
    @pytest.fixture
    def db_session(self):
        """Create in-memory database session for testing"""
        engine = create_engine(
            "sqlite:///:memory:",
            connect_args={"check_same_thread": False},
            poolclass=StaticPool,
        )
        Base.metadata.create_all(engine)
        SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
        session = SessionLocal()
        yield session
        session.close()
    
    def test_sensor_configuration_creation(self, db_session):
        """Test SensorConfiguration model creation"""
        # Create sensor configuration
        config_data = SensorConfiguration(
            sensor_id="RFID_001",
            sensor_type="rfid",
            name="RFID Reader 1",
            description="Main RFID reader",
            configuration={"baud_rate": 9600, "timeout": 1000},
            calibration_data={"offset_x": 0.0, "offset_y": 0.0}
        )
        
        db_session.add(config_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_config = db_session.query(SensorConfiguration).filter(
            SensorConfiguration.sensor_id == "RFID_001"
        ).first()
        
        assert retrieved_config is not None
        assert retrieved_config.sensor_id == "RFID_001"
        assert retrieved_config.sensor_type == "rfid"
        assert retrieved_config.name == "RFID Reader 1"
        assert retrieved_config.description == "Main RFID reader"
        assert retrieved_config.configuration == {"baud_rate": 9600, "timeout": 1000}
        assert retrieved_config.calibration_data == {"offset_x": 0.0, "offset_y": 0.0}
        assert retrieved_config.is_active is True
        assert retrieved_config.created_at is not None
        assert retrieved_config.updated_at is not None
    
    def test_sensor_reading_creation(self, db_session):
        """Test SensorReading model creation"""
        # Create sensor reading
        reading_data = SensorReading(
            sensor_id="RFID_001",
            sensor_type="rfid",
            reading_data={"rfid_id": "RFID_001", "signal_strength": 0.8},
            processed_data={"position": {"x": 100, "y": 200}},
            quality_score=0.9
        )
        
        db_session.add(reading_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_reading = db_session.query(SensorReading).filter(
            SensorReading.sensor_id == "RFID_001"
        ).first()
        
        assert retrieved_reading is not None
        assert retrieved_reading.sensor_id == "RFID_001"
        assert retrieved_reading.sensor_type == "rfid"
        assert retrieved_reading.reading_data == {"rfid_id": "RFID_001", "signal_strength": 0.8}
        assert retrieved_reading.processed_data == {"position": {"x": 100, "y": 200}}
        assert retrieved_reading.quality_score == 0.9
        assert retrieved_reading.is_valid is True
        assert retrieved_reading.timestamp is not None
    
    def test_sensor_status_creation(self, db_session):
        """Test SensorStatus model creation"""
        # Create sensor status
        status_data = SensorStatus(
            sensor_id="RFID_001",
            sensor_type="rfid",
            status="online",
            error_count=0,
            health_score=1.0
        )
        
        db_session.add(status_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_status = db_session.query(SensorStatus).filter(
            SensorStatus.sensor_id == "RFID_001"
        ).first()
        
        assert retrieved_status is not None
        assert retrieved_status.sensor_id == "RFID_001"
        assert retrieved_status.sensor_type == "rfid"
        assert retrieved_status.status == "online"
        assert retrieved_status.error_count == 0
        assert retrieved_status.health_score == 1.0
        assert retrieved_status.updated_at is not None
    
    def test_sensor_calibration_creation(self, db_session):
        """Test SensorCalibration model creation"""
        # Create sensor calibration
        calibration_data = SensorCalibration(
            sensor_id="RFID_001",
            sensor_type="rfid",
            calibration_type="offset",
            calibration_data={"offset_x": 0.0, "offset_y": 0.0},
            reference_data={"reference_x": 100.0, "reference_y": 200.0},
            accuracy=0.95
        )
        
        db_session.add(calibration_data)
        db_session.commit()
        
        # Retrieve and verify
        retrieved_calibration = db_session.query(SensorCalibration).filter(
            SensorCalibration.sensor_id == "RFID_001"
        ).first()
        
        assert retrieved_calibration is not None
        assert retrieved_calibration.sensor_id == "RFID_001"
        assert retrieved_calibration.sensor_type == "rfid"
        assert retrieved_calibration.calibration_type == "offset"
        assert retrieved_calibration.calibration_data == {"offset_x": 0.0, "offset_y": 0.0}
        assert retrieved_calibration.reference_data == {"reference_x": 100.0, "reference_y": 200.0}
        assert retrieved_calibration.accuracy == 0.95
        assert retrieved_calibration.is_valid is True
        assert retrieved_calibration.created_at is not None
    
    def test_map_relationships(self, db_session):
        """Test Map model relationships"""
        # Create map
        map_data = Map(
            map_id="test_map_001",
            name="Test Map",
            resolution=50.0,
            width=1000,
            height=1000,
            occupancy_grid=[[-1, 0, 1]],
            robot_trajectory=[],
            rfid_positions=[]
        )
        db_session.add(map_data)
        
        # Create session
        session_data = MapSession(
            session_id="test_session_001",
            map_id="test_map_001"
        )
        db_session.add(session_data)
        
        # Create robot position
        position_data = RobotPosition(
            map_id="test_map_001",
            session_id="test_session_001",
            x=100.0,
            y=200.0,
            theta=1.57,
            confidence=0.9,
            source="hybrid"
        )
        db_session.add(position_data)
        db_session.commit()
        
        # Test relationships
        retrieved_map = db_session.query(Map).filter(Map.map_id == "test_map_001").first()
        
        assert len(retrieved_map.sessions) == 1
        assert retrieved_map.sessions[0].session_id == "test_session_001"
        assert len(retrieved_map.positions) == 1
        assert retrieved_map.positions[0].x == 100.0
        
        retrieved_session = db_session.query(MapSession).filter(
            MapSession.session_id == "test_session_001"
        ).first()
        
        assert retrieved_session.map.map_id == "test_map_001"
        assert len(retrieved_session.positions) == 1
        assert retrieved_session.positions[0].x == 100.0
