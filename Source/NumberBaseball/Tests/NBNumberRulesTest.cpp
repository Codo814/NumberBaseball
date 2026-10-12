#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Game/NBGameModeBase.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNBNumberRulesTest, "NumberBaseball.Game.NumberRules", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNBNumberRulesTest::RunTest(const FString& Parameters)
{
	ANBGameModeBase* GameMode = GetMutableDefault<ANBGameModeBase>();
	// 입력 전체를 검사하여 짧은 입력, 중복, 문자, 0과 접미사 추측을 거절하는지 확인합니다.
	const TArray<FString> InvalidInputs = { TEXT(""), TEXT("12"), TEXT("112"), TEXT("abc"), TEXT("012"), TEXT("1234"), TEXT("abc123"), TEXT(" 123"), TEXT("123 ") };
	for (const FString& Input : InvalidInputs)
	{
		TestFalse(*FString::Printf(TEXT("Reject '%s'"), *Input), GameMode->IsGuessNumberString(Input));
		TestFalse(TEXT("Invalid input has feedback"), GameMode->GetGuessValidationMessage(Input).IsEmpty());
	}
	TestTrue(TEXT("Accept 386"), GameMode->IsGuessNumberString(TEXT("386")));
	// 과제 예시와 자리만 바뀐 입력으로 Strike, Ball, OUT 경계를 확인합니다.
	TestEqual(TEXT("Example 123"), GameMode->JudgeResult(TEXT("386"), TEXT("123")), FString(TEXT("0S1B")));
	TestEqual(TEXT("Winner"), GameMode->JudgeResult(TEXT("386"), TEXT("386")), FString(TEXT("3S0B")));
	TestEqual(TEXT("All balls"), GameMode->JudgeResult(TEXT("386"), TEXT("863")), FString(TEXT("0S3B")));
	TestEqual(TEXT("Mixed"), GameMode->JudgeResult(TEXT("386"), TEXT("368")), FString(TEXT("1S2B")));
	TestEqual(TEXT("Out"), GameMode->JudgeResult(TEXT("386"), TEXT("127")), FString(TEXT("OUT")));
	TestEqual(TEXT("Short input safe"), GameMode->JudgeResult(TEXT("386"), TEXT("12")), FString(TEXT("잘못된 숫자입니다.")));
	// 여러 번 생성해도 정답이 항상 세 자리이며 1~9 중 서로 다른 숫자인지 확인합니다.
	for (int32 i = 0; i < 100; ++i)
	{
		TestTrue(TEXT("Generated secret follows rules"), GameMode->IsGuessNumberString(GameMode->GenerateSecretNumber()));
	}
	return true;
}
#endif