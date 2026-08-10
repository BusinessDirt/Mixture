import logging
import sys


logger = logging.getLogger(__name__)


class PythonConfiguration:
    @classmethod
    def validate(cls, required_version: tuple[int, int]) -> bool:
        if sys.version_info < required_version:
            logger.error(
                "Python version too low, expected version %d.%d or higher.",
                required_version[0],
                required_version[1],
            )
            return False

        logger.info(
            "Python %d.%d.%d detected",
            sys.version_info.major,
            sys.version_info.minor,
            sys.version_info.micro,
        )
        return True
