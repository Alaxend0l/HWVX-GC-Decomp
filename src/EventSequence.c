

void Thing_Release(void *);
void EventSeq_Release(void *arg0)
{
    Thing_Release(arg0);
}
