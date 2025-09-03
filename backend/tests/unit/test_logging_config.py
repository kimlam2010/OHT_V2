"""
Unit tests for logging configuration
"""

import logging
import json
from unittest.mock import patch, MagicMock

from app.core.logging_config import (
    StructuredFormatter,
    CorrelationFilter,
    setup_logging,
    get_logger,
    set_correlation_id,
    get_correlation_id,
    log_with_context,
    PerformanceLogger,
    log_performance,
    RequestLogger
)


class TestStructuredFormatter:
    """Test StructuredFormatter class"""
    
    def test_structured_formatter_creation(self):
        """Test StructuredFormatter creation"""
        formatter = StructuredFormatter()
        assert isinstance(formatter, logging.Formatter)
    
    def test_format_basic_record(self):
        """Test formatting basic log record"""
        formatter = StructuredFormatter()
        record = logging.LogRecord(
            name="test_logger",
            level=logging.INFO,
            pathname="test.py",
            lineno=10,
            msg="Test message",
            args=(),
            exc_info=None
        )
        record.module = "test_module"
        record.funcName = "test_function"
        
        formatted = formatter.format(record)
        log_data = json.loads(formatted)
        
        assert log_data["level"] == "INFO"
        assert log_data["logger"] == "test_logger"
        assert log_data["message"] == "Test message"
        assert log_data["module"] == "test_module"
        assert log_data["function"] == "test_function"
        assert log_data["line"] == 10
        assert "timestamp" in log_data
    
    def test_format_with_extra_fields(self):
        """Test formatting log record with extra fields"""
        formatter = StructuredFormatter()
        record = logging.LogRecord(
            name="test_logger",
            level=logging.INFO,
            pathname="test.py",
            lineno=10,
            msg="Test message",
            args=(),
            exc_info=None
        )
        record.module = "test_module"
        record.funcName = "test_function"
        record.extra_fields = {"user_id": 123, "action": "login"}
        
        formatted = formatter.format(record)
        log_data = json.loads(formatted)
        
        assert log_data["user_id"] == 123
        assert log_data["action"] == "login"


class TestCorrelationFilter:
    """Test CorrelationFilter class"""
    
    def test_correlation_filter_creation(self):
        """Test CorrelationFilter creation"""
        filter_obj = CorrelationFilter()
        assert isinstance(filter_obj, logging.Filter)
    
    def test_filter_adds_correlation_id(self):
        """Test filter adds correlation ID to record"""
        filter_obj = CorrelationFilter()
        record = logging.LogRecord(
            name="test_logger",
            level=logging.INFO,
            pathname="test.py",
            lineno=10,
            msg="Test message",
            args=(),
            exc_info=None
        )
        
        # Set correlation ID
        test_id = "test-correlation-123"
        with patch('app.core.logging_config.correlation_id') as mock_corr_id:
            mock_corr_id.get.return_value = test_id
            
            result = filter_obj.filter(record)
            
            assert result is True
            assert hasattr(record, 'correlation_id')
            assert record.correlation_id == test_id


class TestLoggingSetup:
    """Test logging setup functions"""
    
    @patch('logging.getLogger')
    def test_setup_logging_structured(self, mock_get_logger: MagicMock):
        """Test setup_logging with structured format"""
        mock_root_logger = MagicMock()
        mock_get_logger.return_value = mock_root_logger
        
        setup_logging(level="DEBUG", format_type="structured")
        
        # Verify handlers were added
        mock_root_logger.addHandler.assert_called()
        mock_root_logger.setLevel.assert_called_with(logging.DEBUG)
    
    @patch('logging.getLogger')
    def test_setup_logging_standard(self, mock_get_logger: MagicMock):
        """Test setup_logging with standard format"""
        mock_root_logger = MagicMock()
        mock_get_logger.return_value = mock_root_logger
        
        setup_logging(level="WARNING", format_type="standard")
        
        # Verify handlers were added
        mock_root_logger.addHandler.assert_called()
        mock_root_logger.setLevel.assert_called_with(logging.WARNING)
    
    def test_get_logger(self):
        """Test get_logger function"""
        logger = get_logger("test_logger")
        assert isinstance(logger, logging.Logger)
        assert logger.name == "test_logger"


class TestCorrelationID:
    """Test correlation ID functions"""
    
    def test_set_correlation_id_custom(self):
        """Test set_correlation_id with custom ID"""
        custom_id = "custom-123"
        result = set_correlation_id(custom_id)
        
        assert result == custom_id
        assert get_correlation_id() == custom_id
    
    def test_set_correlation_id_auto(self):
        """Test set_correlation_id with auto-generated ID"""
        result = set_correlation_id()
        
        assert result is not None
        assert isinstance(result, str)
        assert len(result) > 0
        assert get_correlation_id() == result
    
    def test_get_correlation_id_none(self):
        """Test get_correlation_id when none set"""
        # Clear correlation ID
        set_correlation_id(None)
        result = get_correlation_id()
        
        assert result is None


class TestLogWithContext:
    """Test log_with_context function"""
    
    def test_log_with_context(self):
        """Test log_with_context function"""
        logger = logging.getLogger("test_logger")
        
        with patch.object(logger, 'info') as mock_info:
            log_with_context(logger, "INFO", "Test message", user_id=123, action="test")
            
            mock_info.assert_called_once()
            call_args = mock_info.call_args
            assert call_args[0][0] == "Test message"
            assert call_args[1]["extra"]["extra_fields"]["user_id"] == 123
            assert call_args[1]["extra"]["extra_fields"]["action"] == "test"


class TestPerformanceLogger:
    """Test PerformanceLogger class"""
    
    def test_performance_logger_creation(self):
        """Test PerformanceLogger creation"""
        logger = logging.getLogger("test_logger")
        perf_logger = PerformanceLogger(logger, "test_operation")
        
        assert perf_logger.logger == logger
        assert perf_logger.operation == "test_operation"
        assert perf_logger.start_time is not None
    
    def test_performance_logger_context_success(self):
        """Test PerformanceLogger context manager success"""
        logger = logging.getLogger("test_logger")
        
        with patch.object(logger, 'info') as mock_info:
            with PerformanceLogger(logger, "test_operation"):
                pass  # Simulate successful operation
            
            mock_info.assert_called_once()
            call_args = mock_info.call_args
            assert "Operation completed: test_operation" in call_args[0][0]
            assert call_args[1]["extra"]["extra_fields"]["status"] == "success"
    
    def test_performance_logger_context_failure(self):
        """Test PerformanceLogger context manager failure"""
        logger = logging.getLogger("test_logger")
        
        with patch.object(logger, 'error') as mock_error:
            try:
                with PerformanceLogger(logger, "test_operation"):
                    raise ValueError("Test error")
            except ValueError:
                pass
            
            mock_error.assert_called_once()
            call_args = mock_error.call_args
            assert "Operation failed: test_operation" in call_args[0][0]
            assert call_args[1]["extra"]["extra_fields"]["status"] == "failed"
    
    def test_log_performance_function(self):
        """Test log_performance function"""
        logger = logging.getLogger("test_logger")
        perf_logger = log_performance(logger, "test_operation")
        
        assert isinstance(perf_logger, PerformanceLogger)
        assert perf_logger.logger == logger
        assert perf_logger.operation == "test_operation"


class TestRequestLogger:
    """Test RequestLogger class"""
    
    def test_request_logger_creation(self):
        """Test RequestLogger creation"""
        logger = logging.getLogger("test_logger")
        request_logger = RequestLogger(logger)
        
        assert request_logger.logger == logger
    
    def test_log_request(self):
        """Test log_request method"""
        logger = logging.getLogger("test_logger")
        request_logger = RequestLogger(logger)
        
        with patch.object(logger, 'info') as mock_info:
            headers = {"Authorization": "Bearer token"}
            request_logger.log_request("POST", "/api/test", headers, "test body")
            
            mock_info.assert_called_once()
            call_args = mock_info.call_args
            assert call_args[0][0] == "Incoming request"
            assert call_args[1]["extra"]["extra_fields"]["request_method"] == "POST"
            assert call_args[1]["extra"]["extra_fields"]["request_url"] == "/api/test"
    
    def test_log_response(self):
        """Test log_response method"""
        logger = logging.getLogger("test_logger")
        request_logger = RequestLogger(logger)
        
        with patch.object(logger, 'info') as mock_info:
            request_logger.log_response(200, 0.5, 1024)
            
            mock_info.assert_called_once()
            call_args = mock_info.call_args
            assert call_args[0][0] == "Response sent"
            assert call_args[1]["extra"]["extra_fields"]["response_status_code"] == 200
            assert call_args[1]["extra"]["extra_fields"]["response_time_seconds"] == 0.5
    
    def test_log_error(self):
        """Test log_error method"""
        logger = logging.getLogger("test_logger")
        request_logger = RequestLogger(logger)
        
        with patch.object(logger, 'error') as mock_error:
            error = ValueError("Test error")
            context = {"user_id": 123}
            request_logger.log_error(error, context)
            
            mock_error.assert_called_once()
            call_args = mock_error.call_args
            assert "Request error: Test error" in call_args[0][0]
            assert call_args[1]["extra"]["extra_fields"]["error_type"] == "ValueError"
            assert call_args[1]["extra"]["extra_fields"]["error_context"]["user_id"] == 123
